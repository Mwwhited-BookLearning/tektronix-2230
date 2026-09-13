"""
Listen on a serial port and log every byte received, both as hex and
as a printable-ASCII rendering (non-printable bytes shown as '.') -
used to test whether the Tek 2230's self-test banner text is actually
transmitted over RS-232 (validating/refuting the write_readout_port_
byte "genuine UART" hypothesis in disasm/NOTES.md).

Logs raw bytes regardless of whether they decode as clean text, so
even a wrong baud-rate guess still answers "was anything transmitted
at all" - the primary question - while a correct guess would also
show the actual banner text.

Usage: python listen_serial.py [--port COM3] [--baud 9600] [--seconds 60]
"""
import argparse
import sys
import time

import serial


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--port", default="COM3")
    ap.add_argument("--baud", type=int, default=9600)
    ap.add_argument("--seconds", type=float, default=60)
    ap.add_argument("--parity", default="N", choices=["N", "E", "O"])
    ap.add_argument("--bytesize", type=int, default=8, choices=[7, 8])
    ap.add_argument("--stopbits", type=int, default=1, choices=[1, 2])
    args = ap.parse_args()

    parity_map = {"N": serial.PARITY_NONE, "E": serial.PARITY_EVEN, "O": serial.PARITY_ODD}

    print(f"Opening {args.port} @ {args.baud} {args.bytesize}{args.parity}{args.stopbits}, "
          f"listening for {args.seconds}s ...", file=sys.stderr)
    ser = serial.Serial(
        port=args.port,
        baudrate=args.baud,
        bytesize=args.bytesize,
        parity=parity_map[args.parity],
        stopbits=args.stopbits,
        timeout=0.5,
    )

    all_bytes = bytearray()
    start = time.time()
    try:
        while time.time() - start < args.seconds:
            chunk = ser.read(256)
            if chunk:
                all_bytes.extend(chunk)
                ts = time.time() - start
                hexstr = chunk.hex(" ")
                ascii_str = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
                print(f"[{ts:6.2f}s] {len(chunk):3d} bytes: {hexstr}")
                print(f"          ascii: {ascii_str!r}")
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()

    print(f"\nTotal bytes received: {len(all_bytes)}", file=sys.stderr)
    if all_bytes:
        print("Full ASCII rendering:", file=sys.stderr)
        print("".join(chr(b) if 32 <= b < 127 or b in (10, 13) else "." for b in all_bytes))
    else:
        print("Nothing received - either wrong port/baud, the option isn't installed, "
              "or the self-test path doesn't transmit over RS-232.", file=sys.stderr)


if __name__ == "__main__":
    main()
