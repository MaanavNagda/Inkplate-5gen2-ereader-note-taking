#!/usr/bin/env python3
import os
import sys
import time
import serial

PORT = "/dev/ttyUSB0"
BAUD = 460800
CHUNK = 2048
ROOT = "textbooks_out"


def wait_for_line(ser, expected=None, timeout=30):
    start = time.time()
    while time.time() - start < timeout:
        line = ser.readline().decode("utf-8", errors="ignore").strip()
        if line:
            if expected is None or line.startswith(expected):
                return line
    raise TimeoutError(f"timeout waiting for {expected!r}")


def send_file(ser, local, remote, size):
    ser.write(f"UPLOAD {remote} {size}\n".encode())
    wait_for_line(ser, "READY", timeout=10)

    with open(local, "rb") as f:
        sent = 0
        while sent < size:
            data = f.read(CHUNK)
            if not data:
                break
            ser.write(f"CHUNK {len(data)}\n".encode())
            ser.write(data)
            wait_for_line(ser, "ACK", timeout=60)
            sent += len(data)

    wait_for_line(ser, "OK", timeout=30)


def main():
    if not os.path.isdir(ROOT):
        print(f"{ROOT} not found", file=sys.stderr)
        sys.exit(1)

    ser = serial.Serial(PORT, BAUD, timeout=1.0)

    # Drain boot messages and wait for the receiver to be ready.
    for _ in range(20):
        ser.readline()
    ser.write(b"PING\n")
    wait_for_line(ser, "PONG", timeout=10)
    print("Receiver ready")

    files = []
    for dirpath, _, filenames in os.walk(ROOT):
        for fname in filenames:
            local = os.path.join(dirpath, fname)
            remote = "/" + local.replace(os.sep, "/")
            size = os.path.getsize(local)
            files.append((local, remote, size))
    files.sort()

    total = sum(s for _, _, s in files)
    start = time.time()
    transferred = 0

    for local, remote, size in files:
        print(f"Sending {remote} ({size} bytes)")
        send_file(ser, local, remote, size)
        transferred += size
        elapsed = time.time() - start
        rate = transferred / elapsed if elapsed > 0 else 0
        eta = (total - transferred) / rate if rate > 0 else 0
        print(f"  {transferred}/{total} bytes ({100 * transferred / total:.1f}%)  "
              f"{rate:.0f} B/s  ETA {eta / 60:.1f} min")

    print("All files sent")
    ser.close()


if __name__ == "__main__":
    main()
