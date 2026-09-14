"""
Send one or more RS-232 command lines to a live Tek 2230 and log the
raw responses. Companion to `plot_hpgl_to_svg.py` (same serial
settings/lessons - see that file's docstring for the 9600-vs-1200/4800
baud reliability finding).

Each command is written with a CR terminator (per `hardware/manuals/
2230_programming/README.md`'s "Programming the RS-232-C Interface"
section: the interface strips a trailing CR, and multiple commands on
one physical line aren't supported under Option 12) and answered by
reading until the line goes idle for `--idle-timeout` seconds - the
same "quiet means done" heuristic `plot_hpgl_to_svg.py` uses, since the
instrument doesn't send an explicit end-of-response marker.

Usage:
    # One command
    python scope_rs232.py --port COM3 --baud 4800 --cmd "ID?"

    # A whole batch, newline-separated, logged with timestamps
    python scope_rs232.py --port COM3 --baud 4800 --batch-file cmds.txt --log session.txt

    # Check what a given cable/adapter actually presents on the modem
    # control lines, and force RTS/DTR high if it needs that to pass
    # data through at all (see --show-lines/--rts/--dtr/--rtscts below)
    python scope_rs232.py --port COM3 --show-lines --cmd "ID?"

Hardware flow control: the manual only documents *software* (XON/XOFF)
flow control (the `FLOw` command) - nothing in it mentions RTS/CTS or
DTR/DSR, so the 2230 itself isn't known to require or drive them. But
some USB-serial adapters/cables wire (or need) these pins regardless -
either the adapter's own chipset gates transmission on hardware CTS
independent of any software setting, or a cable jumpers RTS to CTS (or
DTR to DSR) so the adapter sees itself as always "clear to send." Both
scripts default to leaving these lines exactly as pyserial/the OS
driver leaves them on open (no explicit control) - use `--show-lines`
to see what a given cable/adapter actually presents, and `--rts`/
`--dtr`/`--rtscts`/`--dsrdtr` to force a specific state if a cable
needs it.
"""
import argparse
import sys
import time


def send_and_read(ser, cmd, idle_timeout, max_wait, settle=0.15):
    ser.reset_input_buffer()
    ser.write(cmd.encode("ascii") + b"\r")
    data = bytearray()
    start = time.time()
    last = time.time()
    # Give the scope a moment to start responding before we start
    # measuring idle time, otherwise a slow-to-start reply looks "done"
    # immediately.
    time.sleep(settle)
    while True:
        chunk = ser.read(256)
        now = time.time()
        if chunk:
            data.extend(chunk)
            last = now
        if data and (now - last) > idle_timeout:
            break
        if (now - start) > max_wait:
            break
    return bytes(data)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", default="COM3")
    ap.add_argument("--baud", type=int, default=4800)
    ap.add_argument("--cmd", help="single command to send")
    ap.add_argument("--batch-file", help="file with one command per line")
    ap.add_argument("--idle-timeout", type=float, default=1.0)
    ap.add_argument("--max-wait", type=float, default=15.0)
    ap.add_argument("--log", help="also append timestamped transcript to this file")
    ap.add_argument("--delay-between", type=float, default=0.3,
                     help="pause between commands in batch mode")
    ap.add_argument("--raw-out", help="save the raw bytes of a single --cmd response here too")
    ap.add_argument("--rtscts", action="store_true",
                     help="enable RTS/CTS hardware flow control (off by default - the manual "
                          "only documents software XON/XOFF flow control for this instrument)")
    ap.add_argument("--dsrdtr", action="store_true",
                     help="enable DSR/DTR hardware flow control (off by default)")
    ap.add_argument("--dtr", choices=["auto", "on", "off"], default="auto",
                     help="force the DTR output line high/low after opening; 'auto' leaves "
                          "whatever pyserial/the OS driver sets by default")
    ap.add_argument("--rts", choices=["auto", "on", "off"], default="auto",
                     help="force the RTS output line high/low after opening; 'auto' leaves "
                          "whatever pyserial/the OS driver sets by default")
    ap.add_argument("--show-lines", action="store_true",
                     help="print CTS/DSR/CD/RI readback after connecting - use this to see "
                          "what a given cable/adapter actually presents")
    args = ap.parse_args()

    import serial
    ser = serial.Serial(port=args.port, baudrate=args.baud, bytesize=8,
                         parity=serial.PARITY_NONE, stopbits=1, timeout=0.3,
                         rtscts=args.rtscts, dsrdtr=args.dsrdtr)
    if args.dtr != "auto":
        ser.dtr = (args.dtr == "on")
    if args.rts != "auto":
        ser.rts = (args.rts == "on")
    if args.show_lines:
        print(f"Line status: CTS={ser.cts} DSR={ser.dsr} CD={ser.cd} RI={ser.ri} "
              f"(output: DTR={ser.dtr} RTS={ser.rts}, "
              f"rtscts={args.rtscts} dsrdtr={args.dsrdtr})", file=sys.stderr)

    logf = open(args.log, "a", encoding="utf-8") if args.log else None

    def emit(cmd, resp):
        # Some responses (binary CURVe? data) contain arbitrary bytes;
        # repr() on a str decoded with errors="replace" can still
        # contain codepoints the Windows console's active codepage
        # can't print, so always go through an ascii-safe encoding for
        # the terminal. The log file (utf-8) keeps the full repr.
        text = resp.decode("ascii", errors="replace")
        line_out = f">>> {cmd}\n<<< {text!r}\n"
        safe_out = line_out.encode("ascii", errors="backslashreplace").decode("ascii")
        print(safe_out)
        if logf:
            logf.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {line_out}\n")
            logf.flush()

    try:
        # Clear any leftover busy/plot state before starting, same
        # defensive move as plot_hpgl_to_svg.py.
        send_and_read(ser, "PLOt ABOrt", 0.5, 2.0)

        if args.cmd:
            resp = send_and_read(ser, args.cmd, args.idle_timeout, args.max_wait)
            emit(args.cmd, resp)
            if args.raw_out:
                with open(args.raw_out, "wb") as f:
                    f.write(resp)
        elif args.batch_file:
            with open(args.batch_file, "r", encoding="utf-8") as f:
                cmds = [ln.strip() for ln in f if ln.strip() and not ln.strip().startswith("#")]
            for cmd in cmds:
                resp = send_and_read(ser, cmd, args.idle_timeout, args.max_wait)
                emit(cmd, resp)
                time.sleep(args.delay_between)
        else:
            print("Need --cmd or --batch-file", file=sys.stderr)
            sys.exit(1)
    finally:
        ser.close()
        if logf:
            logf.close()


if __name__ == "__main__":
    main()
