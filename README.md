# Inkplate 5V2 E-reader / Notes Maker

Multi-application firmware for the Soldered Inkplate 5 Gen2 board.

## Applications

- **E-reader**: text-only EPUB reader with library, bookmarks, font sizes and dark mode.
- **Textbook**: pre-converted PDF page viewer with chapter/subchapter TOC.
- **Notes**: BLE keyboard-driven Markdown editor with sentence-level partial refresh.
- **Notes + Textbook**: 80/20 split view with the textbook page on top and a notes sliver at the bottom.

## Hardware

- Soldered Inkplate 5 Gen2 (ESP32-WROVER-E)
- WAKE button: GPIO 36
- IO/FLASH button: GPIO 0
- 64 GB microSD card
- Aolso folding BLE keyboard (only needed for notes apps)

## Build / flash

This project is built with [PlatformIO](https://platformio.org) using the `pioarduino` platform to match the Soldered Inkplate Arduino library v11.x (Arduino-ESP32 3.3.x / ESP-IDF 5.5).

With the PlatformIO CLI:

```bash
pio run -e inkplate5v2              # build
pio run -e inkplate5v2 -t upload    # build and flash
pio test -e native                   # run host-side unit tests
```

With the PlatformIO extension in VS Code:

1. Open the project folder.
2. Pick the `inkplate5v2` environment.
3. Use the **Build** and **Upload** buttons.

## Project layout

- `plan.txt` — full implementation plan
- `platformio.ini` — project configuration and dependencies
- `lib/ButtonHandler/` — button state machine, unit-testable
- `src/` — firmware: `main.cpp`, `AppManager`, `ButtonInput`, and app skeletons
- `test/test_button/` — unit tests for the button handler

## Current status

The project skeleton and button handler are implemented. Unit tests for the button handler run on the host (`native`) environment. The apps are currently static placeholders; the next steps are the e-reader library viewer, textbook image viewer, BLE keyboard host, notes editor, and split-screen view.
