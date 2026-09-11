# Photo inventory

One row per photo in this directory (`.heic` originals converted to
`.jpg` via `../convert_heic.py`, 25% scale / ~85% quality). Menu-screen
photos are grouped by what they show; scope-idle/reference shots are
noted separately. See `../HARDWARE.md` for the synthesized menu tree
and cross-references into the firmware findings.

## Comm option rear panel / front panel (first batch, named files)

| File | Shows |
|---|---|
| `comm_option_rear_panel.jpg` | Comm option board rear panel: RS-232-C port (DTE+DCE), 10-position PARAMETERS DIP switch, 9-pin AUX connector (pen relay + analog X/Y) |
| `front_panel.jpg` | 2230 front panel control layout (two stacked units + an unrelated bench DMM) |
| `IMG_1412.jpg` | Main system board (A10) interior, top board lifted: CPU (Intel P8088-2), the two 160-3532/160-3633 ROMs, a Sony-branded A/D chip, a PAL10L8 glue-logic chip, and hand-silkscreened signal labels (TRIGGERED, ACQEN A, EOR, ...FULL) - see `../HARDWARE.md` "Main system board interior" |

## ACQ_MODE_SETUP_TABLE menu tree (timestamped batch)

| File | Screen |
|---|---|
| `20260911_005742350_iOS.jpg` | Top level: SELECT_MODE, SWP_LIMIT, WEIGHT, A_TRIG_POS, DISPLAY, DEFAULT, FORMATTING, PLOT, ADVANCED_FUNCTIONS |
| `20260911_005751019_iOS.jpg` | SELECT_MODE: UN-TRIG/TRIG x sweep-speed matrix, SAMPLE/ACCPEAK/AVERAGE/PEAKDET rows |
| `20260911_005821733_iOS.jpg` | SELECT_MODE, same matrix, different cursor position |
| `20260911_010014766_iOS.jpg` | A_TRIG_POS 512/4K, "Points before trigger, PRE or POST" |
| `20260911_010027277_iOS.jpg` | A_TRIG_POS 480/4K (adjusting) |
| `20260911_010028774_iOS.jpg` | A_TRIG_POS 828/4K (adjusting) |
| `20260911_010029586_iOS.jpg` | A_TRIG_POS 528/4K (adjusting) |
| `20260911_010109329_iOS.jpg` | DISPLAY: DELTA_T_MODE, VECTORS, SMOOTH - "Display formatting" |
| `20260911_010115569_iOS.jpg` | DELTA_T_MODE: DELTA_TIME(default)/1/DELTA_TIME - "delta time or 1/delta_time" |
| `20260911_010117677_iOS.jpg` | VECTORS: OFF/ON(default) - "display line or dots" |
| `20260911_010119590_iOS.jpg` | SMOOTH: OFF/ON(default) |
| `20260911_010144280_iOS.jpg` | DEFAULT: "CURSOR SELECT restores menu defaults" |
| `20260911_010148471_iOS.jpg` | FORMATTING: TARGET_REFERENCE, VGAIN, VPOSITION, HMAG, MODE - "Alter display of SAVE-REFs" |
| `20260911_010150701_iOS.jpg` | TARGET_REFERENCE: SAVE REF 1-1K CH1, "No signal acquired" |
| `20260911_010152684_iOS.jpg` | VGAIN: 1mV, "FUNCTION NOT AVAILABLE IN XY MODE" |
| `20260911_010154374_iOS.jpg` | VPOSITION submenu |
| `20260911_010156149_iOS.jpg` | HMAG: 1X, "FUNCTION NOT AVAILABLE IN XY MODE", "CUR knob for 10X MAG" |
| `20260911_010229224_iOS.jpg` | PLOT: SPEED, START, GRATICULE, AUTO, FORMAT, SETUP - "Control the XY plotter" |
| `20260911_010232964_iOS.jpg` | SPEED 1 - "Set pen speed for plot" |
| `20260911_010244772_iOS.jpg` | START - "Press CURSOR SELECT to Start a PLOT" |
| `20260911_010248524_iOS.jpg` | GRATICULE: OFF(default)/ON - "Enable plotting of graticule" |
| `20260911_010250887_iOS.jpg` | AUTO: OFF(default)/ON - "Select AUTO-PLOT mode" |
| `20260911_010253268_iOS.jpg` | FORMAT: XY, HPGL(default), EPS7, EPS8, TJET, RESERVED - "Select plotter control" |
| `20260911_010256250_iOS.jpg` | SETUP - "Set plotter gain and offset" |
| `20260911_010333533_iOS.jpg` | FORMAT/XY selected - "Analog XY plotter" |
| `20260911_010335235_iOS.jpg` | FORMAT/HPGL - "COMM port, HPGL format" |
| `20260911_010336843_iOS.jpg` | FORMAT/EPS7 - "COMM port, Epson 7 bit format" |
| `20260911_010338400_iOS.jpg` | FORMAT/EPS8 - "COMM port, Epson 8 bit format" |
| `20260911_010340038_iOS.jpg` | FORMAT/TJET - "COMM port, HP ThinkJet format" |
| `20260911_010341518_iOS.jpg` | FORMAT/RESERVED (no description) |
| `20260911_010404655_iOS.jpg` | ADVANCED_FUNCTIONS: REFERENCE, COMM, ACQ_MODE_SETUP_TREE, DIAGNOSTICS |
| `20260911_010406949_iOS.jpg` | REFERENCE: COPY, DELETE, PROTECT - "Change non-volatile ref memories" |
| `20260911_010410539_iOS.jpg` | COMM: DATA, STOP_BITS, FLOW - "Setup host communications" |
| `20260911_010412954_iOS.jpg` | ACQ_MODE_SETUP_TREE: REPETITIVE, FAST_RECORD, SLOW_RECORD, SLOW_TRIGGERED, SLOW_UNTRIGGERED - "Tree structured acquisition setup" |
| `20260911_010414653_iOS.jpg` | DIAGNOSTICS: TESTS, CAL_AIDS, EXERCISERS |
| `20260911_010418307_iOS.jpg` | REFERENCE/COPY: 26-slot A-Z reference bank, "Select SOURCE, Free: 26" |
| `20260911_010420866_iOS.jpg` | REFERENCE/DELETE, Free: 26 |
| `20260911_010422922_iOS.jpg` | REFERENCE/PROTECT: LOCK/UNLOCK, Free: 26 |
| `20260911_010425737_iOS.jpg` | REFERENCE/COPY again (source select) |
| `20260911_010436712_iOS.jpg` | COMM/DATA: ENCDG, SOURCE, CHANNEL - "Setup data transmission parameters" |
| `20260911_010439502_iOS.jpg` | DATA/ENCDG: ASCII, BINARY(default), HEX - "Select coding format" |
| `20260911_010446827_iOS.jpg` | DATA/SOURCE: ACQ - "Use cursor to select source" |
| `20260911_010451018_iOS.jpg` | DATA/SOURCE: REF1 |
| `20260911_010452535_iOS.jpg` | DATA/SOURCE: REF3 |
| `20260911_010457452_iOS.jpg` | DATA/CHANNEL: CH1(default)/CH2 - "Select channel" |
| `20260911_010511248_iOS.jpg` | COMM/STOP_BITS: 1 - "Use cursor control to adjust" |
| `20260911_010519803_iOS.jpg` | COMM/FLOW: OFF - "Use cursor to select flow control" |
| `20260911_010523751_iOS.jpg` | COMM/FLOW: OFF (duplicate) |
| `20260911_010532561_iOS.jpg` | ACQ_MODE_SETUP_TREE/REPETITIVE: SAMPLE/ACCPEAK/AVERAGE(default) - ".05 to 2 uSec/Div" |
| `20260911_010534878_iOS.jpg` | .../FAST_RECORD: SAMPLE(default)/ACCPEAK/AVERAGE - "5 to 10 uSec/Div" |
| `20260911_010536800_iOS.jpg` | .../SLOW_RECORD: SAMPLE/ACCPEAK/AVERAGE/PEAKDET(default) - "20uSec to 50 mSec/Div" |
| `20260911_010538705_iOS.jpg` | .../SLOW_TRIGGERED: .../PEAKDET(default) - "0.1 to 5 Sec/Div or EXT CLOCK" |
| `20260911_010540880_iOS.jpg` | .../SLOW_UNTRIGGERED: SAMPLE/PEAKDET(default) - "0.1 to 5 Sec/Div or EXT CLOCK" |
| `20260911_010550511_iOS.jpg` | DIAGNOSTICS/TESTS: MEMORY, SYSTEM, ACQUISITION |
| `20260911_010553014_iOS.jpg` | DIAGNOSTICS/CAL_AIDS: BOX, CAL_V_POS, CAL_CLK_DLY |
| `20260911_010554880_iOS.jpg` | DIAGNOSTICS/EXERCISERS: CONFIGURATION, IO, A_TO_D_TESTS |
| `20260911_010559479_iOS.jpg` | TESTS/MEMORY: SYS_ROM_0, SYS_ROM_1, COMM_ROM, ACQ_RAM |
| `20260911_010626647_iOS.jpg` | TESTS/MEMORY: COMM_ROM **PASSED** |
| `20260911_010656659_iOS.jpg` | TESTS/MEMORY: ACQ_RAM **PASSED** |
| `20260911_010723325_iOS.jpg` | TESTS/SYSTEM: ACQ_ACCESS, PRC_READBACK, FP_AtoD, COMM_READBACK, COMM_LOOPBACK |
| `20260911_010727112_iOS.jpg` | TESTS/SYSTEM: ACQ_ACCESS FF0 **PASS** |
| `20260911_010730299_iOS.jpg` | TESTS/SYSTEM: PRC_READBACK FE0 **PASS** |
| `20260911_010732356_iOS.jpg` | TESTS/SYSTEM: FP_AtoD **PASSED** |
| `20260911_010734482_iOS.jpg` | TESTS/SYSTEM: COMM_READBACK **PASSED** |
| `20260911_010736452_iOS.jpg` | TESTS/SYSTEM: COMM_LOOPBACK **UNTESTED** |
| `20260911_010751292_iOS.jpg` | TESTS/ACQUISITION: HS_ACQ **PASSED**, TB_DIVIDER, MAX_MIN_ACQ, XY_ACQ, CLK_DELAY |
| `20260911_010753843_iOS.jpg` | TESTS/ACQUISITION: TB_DIVIDER 1K0 **PASS** |
| `20260911_010755767_iOS.jpg` | TESTS/ACQUISITION: MAX_MIN_ACQ **PASSED** |
| `20260911_010757528_iOS.jpg` | TESTS/ACQUISITION: XY_ACQ **PASSED** |
| `20260911_010759208_iOS.jpg` | TESTS/ACQUISITION: CLK_DELAY **PASSED** |
| `20260911_010919509_iOS.jpg` | Scope idle trace (5V/5V/0.2ms), full stacked-unit context shot |
| `20260911_010927036_iOS.jpg` | Scope idle trace (2V/5V/0.2ms) |
| `20260911_010930632_iOS.jpg` | Scope idle trace (1V/5V/0.2ms) |
