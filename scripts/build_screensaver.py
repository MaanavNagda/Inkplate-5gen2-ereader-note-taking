#!/usr/bin/env python3
"""
Convert the project-root screensaver JPEG into a 3-bit-grayscale-friendly
720x1280 JPEG and embed it as a C source/header pair.

Output: src/screensaver.h and src/screensaver.cpp
"""

import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src")


def find_input():
    for name in ["Screensaver.jpeg", "screensaver.jpeg", "Screensaver.jpg", "screensaver.jpg"]:
        p = os.path.join(ROOT, name)
        if os.path.exists(p):
            return p
    return None


def main():
    infile = find_input()
    if not infile:
        print("No Screensaver.jpeg/screensaver.jpg found in project root.", file=sys.stderr)
        sys.exit(1)

    jpg = os.path.join(ROOT, "screensaver_3bit.jpg")

    # Resize/crop to the portrait e-paper resolution and keep JPEG small.
    # Quality 75 keeps a good 3-bit dithered result while staying under ~100 KB.
    cmd = [
        "magick", infile,
        "-resize", "720x1280^",
        "-gravity", "center",
        "-extent", "720x1280",
        "-quality", "75",
        "jpg:" + jpg,
    ]
    subprocess.run(cmd, check=True)

    with open(jpg, "rb") as f:
        data = f.read()

    print(f"Processed screensaver: {len(data)} bytes")

    hpath = os.path.join(SRC, "screensaver.h")
    cpath = os.path.join(SRC, "screensaver.cpp")

    with open(hpath, "w") as hf:
        hf.write("#ifndef SCREENSAVER_H\n")
        hf.write("#define SCREENSAVER_H\n\n")
        hf.write("#include <cstdint>\n")
        hf.write("#include <cstddef>\n\n")
        hf.write("extern const std::size_t screensaver_jpg_len;\n")
        hf.write("extern const unsigned char screensaver_jpg[];\n\n")
        hf.write("#endif\n")

    with open(cpath, "w") as cf:
        cf.write('#include "screensaver.h"\n\n')
        cf.write(f"const std::size_t screensaver_jpg_len = {len(data)};\n")
        cf.write("const unsigned char screensaver_jpg[] = {\n")
        for i in range(0, len(data), 16):
            chunk = data[i : i + 16]
            line = ", ".join(f"0x{b:02x}" for b in chunk)
            cf.write("    " + line)
            cf.write(",\n" if i + 16 < len(data) else "\n")
        cf.write("};\n")

    print(f"Generated src/screensaver.h / .cpp ({len(data)} bytes)")
    os.remove(jpg)


if __name__ == "__main__":
    main()
