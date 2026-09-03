#include "ButtonInput.h"

void ButtonInput::begin() {
    // GPIO 36 is an input-only pin and does not have an internal pull-up.
    // The Inkplate 5V2 schematic provides an external pull-up for the wake-up button.
    // GPIO 0 has an internal pull-up for the other / flash button.
    pinMode(WAKE_PIN, INPUT_PULLUP);
    pinMode(IO_PIN, INPUT);
}

void ButtonInput::update() {
    const uint32_t now = millis();

    bool rWake = digitalRead(WAKE_PIN) == LOW;
    bool rIo = digitalRead(IO_PIN) == LOW;

    if (rWake != rawWake_) {
        rawWake_ = rWake;
        lastWakeChange_ = now;
    }
    if (rIo != rawIo_) {
        rawIo_ = rIo;
        lastIoChange_ = now;
    }

    if (now - lastWakeChange_ >= DEBOUNCE_MS) {
        stableWake_ = rawWake_;
    }
    if (now - lastIoChange_ >= DEBOUNCE_MS) {
        stableIo_ = rawIo_;
    }
}
