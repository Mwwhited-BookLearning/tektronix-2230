"""
Interactive serial console for manually probing the Tek 2230's RS-232
comm option: type a command line, it's sent with a terminator, and any
response is printed as it arrives (hex + ASCII). Meant to be run by
the user directly (in its own terminal window) while operating the
scope, since it needs a human typing commands and watching the CRT/
front panel at the same time.

Background: STRINGS.md's "GPIB command/parameter keyword table" (found
in the comm ROM, 117 keywords) is almost certainly this instrument's
actual GPIB/RS-232 command language - e.g. CURVe, WAVfrm, WFMpre, DATa,
ENCdg, CHAnnel, SOUrce, ACQuisition, STArt, STOP, SET(implied by
DEFault/RESet keywords). The literal string "TEK/2230 SYS:0.0,COMM:0.0"
also found in the comm ROM is the likely ID?-style response banner.

Usage:
    python serial_console.py --port COM3 --baud 600

Then type commands and press Enter. Suggested first tries (terminator
defaults to LF - use --terminator CRLF if the scope wants CR+LF):
    ID?
    SET?
    WAVFRM?
    CURVE?
    DATA ENCDG:ASCII
    DATA SOURCE:CH1

Type "quit" to exit.
"""
import argparse
import sys
import threading
import time

import serial


def reader_thread(ser, stop_event):
    while not stop_event.is_set():
        chunk = ser.read(256)
        if chunk:
            hexstr = chunk.hex(" ")
            ascii_str = "".join(chr(b) if 32 <= b < 127 or b in (10, 13) else "." for b in chunk)
            print(f"\n<< {len(chunk)} bytes: {hexstr}")
            print(f"<< ascii: {ascii_str!r}")
            print("> ", end="", flush=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--port", default="COM3")
    ap.add_argument("--baud", type=int, default=600)
    ap.add_argument("--parity", default="N", choices=["N", "E", "O"])
    ap.add_argument("--bytesize", type=int, default=8, choices=[7, 8])
    ap.add_argument("--stopbits", type=int, default=1, choices=[1, 2])
    ap.add_argument("--terminator", default="LF", choices=["LF", "CRLF", "CR", "NONE"])
    args = ap.parse_args()

    parity_map = {"N": serial.PARITY_NONE, "E": serial.PARITY_EVEN, "O": serial.PARITY_ODD}
    term_map = {"LF": b"\n", "CRLF": b"\r\n", "CR": b"\r", "NONE": b""}
    terminator = term_map[args.terminator]

    print(f"Opening {args.port} @ {args.baud} {args.bytesize}{args.parity}{args.stopbits}, "
          f"terminator={args.terminator!r} ...", file=sys.stderr)
    ser = serial.Serial(
        port=args.port,
        baudrate=args.baud,
        bytesize=args.bytesize,
        parity=parity_map[args.parity],
        stopbits=args.stopbits,
        timeout=0.2,
    )

    stop_event = threading.Event()
    t = threading.Thread(target=reader_thread, args=(ser, stop_event), daemon=True)
    t.start()

    print("Type a command and press Enter (or 'quit' to exit).")
    try:
        while True:
            line = input("> ")
            if line.strip().lower() == "quit":
                break
            payload = line.encode("ascii", errors="replace") + terminator
            ser.write(payload)
            print(f">> sent {len(payload)} bytes: {payload!r}")
    except (KeyboardInterrupt, EOFError):
        pass
    finally:
        stop_event.set()
        time.sleep(0.3)
        ser.close()


if __name__ == "__main__":
    main()
