# Proposal: schematic-to-netlist extraction via SINA + Ollama

Written up 2026-09-18 for later review - not started yet. Target
hardware: an RTX 4070 Ti (12GB VRAM). Candidate tool: **SINA**
(University of Utah / University of Colorado Boulder, Jul 2026).

## Why

This session found a real, unresolved conflict between the Tektronix
Options manual's own documented Status Buffer bit map and what the
comm-board self-test's disassembly actually requires (bit 6 vs bit 7
for the `DIAG`/`3D` loopback - see `MEMORY_MAP.md`'s Interrupt Mask
Latch section), plus two undocumented jumpers (`P9107`, `P9105`) whose
electrical function is still unknown (see `TODO.md`'s jumper-hunt
item). A manual read of the two relevant schematic pages (65 and 114
of the Service Manual's Section 9, Diagrams) confirmed component
designators and signal labels, but couldn't reliably trace wire-level
connectivity through crossings - see `docs/diagrams-index.md`'s own
explicit caveat about that limitation. That's exactly the class of
problem automated schematic-to-netlist tools target.

## Candidate tool

[SINA](https://arxiv.org/abs/2607.01609) claims 96.5% netlist-
generation accuracy on IC- and PCB-level schematics via a four-stage
pipeline: YOLOv8 component detection -> connected-component-labeling
for wire connectivity -> EasyOCR for text/reference designators -> a
VLM call to assemble the final netlist and resolve ambiguous
reference-designator assignments. Code is published anonymously at
`anonymous.4open.science/r/SINA-213F` pending de-anonymization (peer
review is still in progress as of this writing).

The alternative surveyed, **Img2SimV2**, has no public repository
found - ruled out for now.

## Key finding: the OpenAI call is in the script, not the model

The detection stage (`current_trained_model`, a pretrained YOLOv8
checkpoint) is fully local already - no code changes needed there, and
it barely touches the 4070 Ti's 12GB. The netlist-assembly stage is a
different story: `helper_files/generate_netlists.py` makes a raw HTTP
call, not even through the `openai` SDK:

```python
api_key = os.getenv("OPENAI_API_KEY")
...
"model": "gpt-4o",
...
requests.post("https://api.openai.com/v1/chat/completions", headers=..., json=...)
```

Because it's a hardcoded `requests.post` to a fixed URL rather than an
SDK client, swapping it for a local model is mechanical: Ollama
exposes an OpenAI-compatible endpoint at
`http://localhost:11434/v1/chat/completions` that accepts the same
request shape for vision-capable models.

| | As shipped | Proposed (Ollama) |
|---|---|---|
| Detection | local (YOLOv8) | local (unchanged) |
| OCR | local (EasyOCR) | local (unchanged) |
| Netlist assembly | **cloud** - `gpt-4o` via OpenAI API | **local** - e.g. `llama3.2-vision` or `qwen2.5vl` via Ollama |
| Cost | per-page API usage; images leave the machine | one-time model pull; nothing leaves the machine |

**Open risk, not yet verified**: swapping the endpoint is trivial;
matching quality is not guaranteed. `generate_netlists.py` imports `re`
and `json` and almost certainly parses `gpt-4o`'s response text with
logic tuned to that model's phrasing/formatting. A smaller local
vision model may answer the same prompt in a different shape, silently
breaking the parser even after the URL/model-name swap works. Before
trusting any output, pull the rest of that function (the actual prompt
text and full request payload) and check for OpenAI-specific
parameters (e.g. a `response_format` mode) that Ollama's compatibility
layer may not honor identically.

## Phased plan

### Phase 0 - decide before touching code (~15 min)

Confirm scope and licensing. The repo is still anonymized (pending
review) - check its license once de-anonymized before relying on it
for anything beyond personal experimentation. Decide up front whether
the Ollama conversion is worth doing at all versus just accepting the
OpenAI cost for a handful of pages - two schematic pages a month is a
trivial API bill; the local conversion only pays off if this becomes a
recurring, larger-scale workflow (e.g. the full 126-page Diagrams
manual).

### Phase 1 - environment (~30-45 min)

Stand up the pipeline as-is (cloud), before converting anything:

```
git clone <SINA repo>
cd "Netlist Generator/IC"
python3.12 -m venv env_netlist
env_netlist\Scripts\activate
pip install -r requirements.txt   # CUDA-matched torch wheel for your driver
```

Verify `current_trained_model` contains real weight files (anonymized
repos sometimes strip large binaries on export) before doing anything
else. Get one real end-to-end run working against the OpenAI API first
- this validates the rest of the pipeline independent of the Ollama
conversion, so a bad result later is known to be the model swap, not a
setup mistake.

### Phase 2 - calibration (cheap, low-stakes)

Run it on a page already read by hand: page 8 (CH1/CH2 Attenuators,
`docs/theory-of-operation.md`). Compare SINA's detected components and
reference designators against that known-good reading before trusting
it on anything new. If detection quality is visibly poor here, it
won't do better on the harder pages.

### Phase 3 - convert to Ollama (the actual engineering)

1. Pull a vision-capable model in Ollama (`ollama pull llama3.2-vision`
   or an equivalent qwen2.5vl build).
2. Point `generate_netlists.py`'s request at
   `http://localhost:11434/v1/chat/completions`, swap the model name,
   drop the API-key requirement.
3. Re-run the Phase 2 calibration page and diff the output against the
   cloud run from Phase 1 - same page, two models, side by side.
4. Adjust prompt/parsing only if the diff shows a real quality or
   format regression, not preemptively.

### Phase 4 - the real targets (concrete pass/fail bar)

- **Page 114** (Diagram 23, RS-232 Option Board): success = the
  extracted netlist shows Interrupt Mask Latch output `3D` wired to a
  specific Status Buffer input bit - resolving whether it's bit 6
  (`DIAG`, per the manual's Table 7-36) or bit 7 (matching what the
  firmware's own self-test arithmetic requires).
- **Page 65** (Diagram 14, Microprocessor & Store Panel Controls):
  success = the netlist shows what circuit each position of jumper
  `P9107` (ON/OFF) and `P9105` (TEST/NORM) actually connects to or
  interrupts.

Every claimed connection is a hypothesis to verify against
already-confirmed facts (e.g. the Options manual's own prose, "RLSD is
generated by Interrupt Mask Latch U1236") before it gets written into
`MEMORY_MAP.md` as fact.

### Phase 5 - scale, only if Phase 4 actually works

If both target pages produce genuinely correct, independently-
verifiable results: expand to the rest of the 34 pages already flagged
in `docs/diagrams-index.md`, then consider the full 126-page Diagrams
manual. If not: the manual read-by-eye approach that already produced
the OCR transcriptions and the diagram survey remains the fallback -
this was always an experiment layered on top of a working process, not
a replacement for it.

## Open questions for later

- Is the anonymized repo's license compatible with personal/non-
  commercial use once de-anonymized?
- Does `current_trained_model`'s YOLOv8 checkpoint generalize at all
  to a 1986 photocopied schematic style, or was it trained on cleaner
  modern CAD-drawn schematics? (Phase 2 answers this directly.)
- What exactly is in the rest of `generate_netlists.py` - the full
  prompt, the response-parsing regex, any OpenAI-specific request
  parameters?
- Is a 7-13B local vision model via Ollama actually capable of the
  reference-designator disambiguation task `gpt-4o` was chosen for, or
  does that specific step need a larger model than fits comfortably on
  12GB VRAM?
