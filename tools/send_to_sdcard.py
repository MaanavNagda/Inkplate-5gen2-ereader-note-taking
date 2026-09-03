#!/usr/bin/env python3
"""Send a file from the host to the Inkplate microSD over the USB/serial port."""

import argparse
import os
import re
import serial
import sys
import time

BAUD = 460800
CHUNK = 128
PING_TIMEOUT = 30.0


def safe_name(name: str) -> str:
    """Replace spaces and special characters so the file can live on a FAT SD."""
    base = os.path.splitext(name)[0]
    return re.sub(r"[^A-Za-z0-9_\-]", "_", base) + os.path.splitext(name)[1]


def wait_for_line(ser: serial.Serial, expected: bytes, timeout: float = 10.0) -> bytes:
    start = time.time()
    line = b""
    while time.time() - start < timeout:
        if ser.in_waiting:
            c = ser.read(1)
            if c == b"\n":
                stripped = line.rstrip(b"\r")
                if stripped:
                    print("  <-", stripped.decode("ascii", errors="replace"))
                if stripped.startswith(expected):
                    return stripped
                line = b""
            else:
                line += c
        else:
            time.sleep(0.005)
    return b""


def send_file(port: str, source: str, dest: str):
    size = os.path.getsize(source)
    if size == 0:
        print("Source file is empty.", file=sys.stderr)
        sys.exit(1)

    print(f"Opening {port} at {BAUD} baud...")
    with serial.Serial(port, BAUD, timeout=0.1, rtscts=False, dsrdtr=False) as ser:
        # Give the device time after opening the port.
        time.sleep(0.5)

        # Wait for the device to respond to a PING.
        print("Waiting for device to wake...")
        pong = b""
        deadline = time.time() + PING_TIMEOUT
        while time.time() < deadline and not pong:
            ser.write(b"PING\n")
            pong = wait_for_line(ser, b"PONG", timeout=1.0)
        if not pong:
            print("Device did not respond. Press WAKE/IO to wake it and try again.", file=sys.stderr)
            sys.exit(1)

        print(f"Uploading {os.path.basename(source)} -> {dest} ({size} bytes)")
        cmd = f"UPLOAD {dest} {size}\n".encode("ascii")
        ser.write(cmd)

        ready = wait_for_line(ser, b"READY", timeout=10.0)
        if not ready:
            err = wait_for_line(ser, b"ERROR", timeout=2.0)
            print(f"Device did not accept upload: {err.decode('ascii', errors='replace')}", file=sys.stderr)
            sys.exit(1)

        sent = 0
        start = time.time()
        with open(source, "rb") as f:
            while sent < size:
                chunk_size = min(CHUNK, size - sent)
                data = f.read(chunk_size)
                ser.write(f"CHUNK {chunk_size}\n".encode("ascii"))
                ser.write(data)

                ack = wait_for_line(ser, b"ACK", timeout=60.0)
                if not ack:
                    err = wait_for_line(ser, b"ERROR", timeout=2.0)
                    print(f"No ACK. Error: {err.decode('ascii', errors='replace')}", file=sys.stderr)
                    sys.exit(1)

                sent += chunk_size
                if sent % (CHUNK * 25) == 0 or sent == size:
                    pct = sent * 100 // size
                    elapsed = time.time() - start
                    rate = sent / max(elapsed, 0.001)
                    print(f"  {pct}% ({sent}/{size} bytes)  {rate:.0f} B/s")

        ok = wait_for_line(ser, b"OK", timeout=30.0)
        if not ok:
            err = wait_for_line(ser, b"ERROR", timeout=2.0)
            print(f"Upload did not finish: {err.decode('ascii', errors='replace')}", file=sys.stderr)
            sys.exit(1)

        elapsed = time.time() - start
        print(f"Done. {sent} bytes in {elapsed:.1f}s ({sent / max(elapsed, 0.001):.0f} B/s)")


def main():
    parser = argparse.ArgumentParser(description="Send a file to the Inkplate microSD.")
    parser.add_argument("source", help="Path to the local file to send")
    parser.add_argument("--port", default="/dev/ttyUSB0", help="Serial port")
    parser.add_argument("--dest", help="Destination path on the SD card (default: /books/<safe_name>)")
    args = parser.parse_args()

    if not os.path.isfile(args.source):
        print(f"Source not found: {args.source}", file=sys.stderr)
        sys.exit(1)

    dest = args.dest
    if not dest:
        dest = "/books/" + safe_name(os.path.basename(args.source))

    send_file(args.port, args.source, dest)


if __name__ == "__main__":
    main()
