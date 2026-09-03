#ifndef BUTTON_INPUT_H
#define BUTTON_INPUT_H

#include <Arduino.h>
#include <cstdint>

// Reads and debounces the two on-board buttons.
// The physical "wake up" button (opposite USB-C) is GPIO 36 and is used as the
// primary IO input (it has an external pull-up and is input-only).
// The other button / pin is GPIO 0 (IO/FLASH/power switch), with internal pull-up.
class ButtonInput {
public:
    static constexpr uint8_t WAKE_PIN = 0;
    static constexpr uint8_t IO_PIN = 36;

    void begin();
    void update();
    bool wakePressed() const { return stableWake_; }
    bool ioPressed() const { return stableIo_; }

private:
    bool rawWake_ = false;
    bool rawIo_ = false;
    bool stableWake_ = false;
    bool stableIo_ = false;
    uint32_t lastWakeChange_ = 0;
    uint32_t lastIoChange_ = 0;
    static constexpr uint32_t DEBOUNCE_MS = 20;
};

#endif
