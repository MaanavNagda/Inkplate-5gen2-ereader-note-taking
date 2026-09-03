#!/usr/bin/env python3
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

DISPLAY_WIDTH = 720
DISPLAY_HEIGHT = 1280
SAMPLE_PAGES = 5
QUALITY_OPTIONS = [75, 85, 95]


def c_escape(s: str) -> str:
    out = []
    for ch in s:
        if ch == "\\":
            out.append("\\\\")
        elif ch == '"':
            out.append('\\"')
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\t":
            out.append("\\t")
        elif ch == "\r":
            pass
        elif ord(ch) < 0x20:
            out.append(" ")
        else:
            out.append(ch)
    return "".join(out)


def safe_path_name(name: str) -> str:
    name = re.sub(r"[^0-9A-Za-z_-]", "_", name)
    name = re.sub(r"_+", "_", name)
    return name.strip("_")[:64]


def page_filename(page: int) -> str:
    return f"{page:03d}.jpg"


def render_page(pdf_path: Path, page: int, out_path: Path, quality: int) -> None:
    tmp_dir = out_path.parent / f".tmp_{page}"
    tmp_dir.mkdir(parents=True, exist_ok=True)
    try:
        cmd = [
            "pdftoppm", "-f", str(page), "-l", str(page), "-jpeg",
            "-jpegopt", f"quality={quality}", "-scale-to", str(DISPLAY_HEIGHT),
            "-forcenum", str(pdf_path), str(tmp_dir / "p")
        ]
        subprocess.run(cmd, check=True, capture_output=True, timeout=120)
        produced = list(tmp_dir.glob("p-*.jpg"))
        if not produced:
            produced = list(tmp_dir.glob("p*.jpg"))
        if not produced:
            raise RuntimeError(f"pdftoppm produced no output for page {page}")
        raw = produced[0]
        subprocess.run([
            "magick", str(raw), "-resize", f"{DISPLAY_WIDTH}x{DISPLAY_HEIGHT}>",
            "-quality", str(quality), str(out_path)
        ], check=True, capture_output=True, timeout=120)
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


def pick_quality(pdf_path: Path, page_count: int, pdf_size: int) -> int:
    target = pdf_size / page_count
    upper = target * 1.20
    best_q = QUALITY_OPTIONS[0]
    best_avg = 0.0
    for q in QUALITY_OPTIONS:
        sizes = []
        for p in range(1, min(page_count, SAMPLE_PAGES) + 1):
            sample = pdf_path.parent / f".sample_{q}_{p}.jpg"
            render_page(pdf_path, p, sample, q)
            sizes.append(sample.stat().st_size)
            sample.unlink()
        avg = sum(sizes) / len(sizes)
        print(f"  quality {q}: avg {avg:.0f} B (target {target:.0f}, upper {upper:.0f})")
        if avg <= upper and avg > best_avg:
            best_q = q
            best_avg = avg
    return best_q


def build_textbooks(project_dir: Path) -> None:
    textbooks_dir = project_dir / "textbooks"
    out_dir = project_dir / "textbooks_out"
    output_file = project_dir / "src" / "apps" / "TextbookData.cpp"

    pdf_files = sorted(textbooks_dir.glob("*.pdf")) if textbooks_dir.exists() else []
    if not pdf_files:
        print("No PDFs found in textbooks/, generating empty TextbookData.cpp")

    out = ['#include "TextbookData.h"', "", "namespace textbook {", ""]

    entry_lines = []
    for idx, pdf_path in enumerate(pdf_files):
        info = subprocess.run(
            ["pdfinfo", str(pdf_path)],
            capture_output=True, text=True, check=False, timeout=30
        )
        m = re.search(r"^Pages:\s+(\d+)", info.stdout, re.MULTILINE)
        page_count = int(m.group(1)) if m else 0
        pdf_size = pdf_path.stat().st_size
        safe = safe_path_name(pdf_path.stem)

        print(f"Processing {pdf_path.name} ({page_count} pages, {pdf_size} bytes)")

        book_dir = out_dir / safe
        if book_dir.exists():
            shutil.rmtree(book_dir)
        book_dir.mkdir(parents=True, exist_ok=True)

        quality = pick_quality(pdf_path, page_count, pdf_size)
        print(f"  selected JPEG quality {quality}")

        for p in range(1, page_count + 1):
            out_path = book_dir / page_filename(p)
            print(f"  page {p:03d}/{page_count:03d}", end="\r", flush=True)
            render_page(pdf_path, p, out_path, quality)
        print()

        total_size = sum(f.stat().st_size for f in book_dir.glob("*.jpg"))
        print(f"  wrote {page_count} pages to {book_dir}, total {total_size} bytes")

        title = pdf_path.stem
        var = f"tb{idx}"
        out.append(f'const char* {var}_title = "{c_escape(title)}";')
        out.append(f"const size_t {var}_pageCount = {page_count};")
        out.append(f'const char* {var}_pathPrefix = "/textbooks_out/{safe}/";')
        out.append("")
        entry_lines.append(f"    {{ {var}_title, {var}_pageCount, {var}_pathPrefix }},")

    out.append("const Entry entries[] = {")
    out.extend(entry_lines)
    out.append("};")
    out.append("")
    out.append(f"const size_t entryCount = {len(pdf_files)};")
    out.append("")
    out.append("} // namespace textbook")
    out.append("")

    output_file.write_text("\n".join(out), encoding="utf-8")
    print(f"Generated {output_file}")


def main() -> None:
    project_dir = None
    env = None
    try:
        Import("env")
        env = globals().get("env")
    except Exception:
        pass
    if env is not None:
        project_dir = Path(env["PROJECT_DIR"])
    else:
        project_dir = Path(__file__).resolve().parent.parent
    build_textbooks(project_dir)


if __name__ == "__main__":
    main()
