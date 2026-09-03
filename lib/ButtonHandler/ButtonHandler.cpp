#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint32_t longPressMs, uint32_t bothWindowMs, uint32_t doubleTapMs)
    : longMs_(longPressMs), bothMs_(bothWindowMs), doubleMs_(doubleTapMs),
      wake_(false), io_(false),
      bothActive_(false), bothReported_(false),
      bothStart_(0),
      queue_{}, head_(0), tail_(0) {}

void ButtonHandler::reset() {
    *this = ButtonHandler(longMs_, bothMs_, doubleMs_);
}

void ButtonHandler::enqueue(ButtonAction action) {
    std::size_t next = (tail_ + 1) % QUEUE_SIZE;
    if (next == head_) {
        // Queue full; drop oldest.
        head_ = (head_ + 1) % QUEUE_SIZE;
    }
    queue_[tail_] = action;
    tail_ = next;
}

bool ButtonHandler::hasAction() const {
    return head_ != tail_;
}

ButtonAction ButtonHandler::getAction() {
    if (head_ == tail_) return ButtonAction::NONE;
    ButtonAction a = queue_[head_];
    head_ = (head_ + 1) % QUEUE_SIZE;
    return a;
}

void ButtonHandler::update(bool wakePressed, bool ioPressed, uint32_t nowMs) {
    wake_ = wakePressed;
    io_ = ioPressed;

    if (wake_ && io_) {
        if (!bothActive_) {
            bothActive_ = true;
            bothStart_ = nowMs;
            // Stop single-button tracking; a fresh press starts when both are released.
            wakeTrack_ = {};
            ioTrack_ = {};
        }
        if (!bothReported_ && (nowMs - bothStart_ >= bothMs_)) {
            bothReported_ = true;
            enqueue(ButtonAction::BOTH);
        }
        return;
    }

    if (bothActive_) {
        bothActive_ = false;
        bothStart_ = 0;
        bothReported_ = false;
        // Cancel any single-button tracking so we do not emit short/long on release.
        wakeTrack_ = {};
        ioTrack_ = {};
    }

    processOne(wakeTrack_, wake_, nowMs,
               ButtonAction::WAKE_SHORT, ButtonAction::WAKE_LONG, ButtonAction::WAKE_DOUBLE);
    processOne(ioTrack_, io_, nowMs,
               ButtonAction::IO_SHORT, ButtonAction::IO_LONG, ButtonAction::IO_DOUBLE);
}

void ButtonHandler::processOne(Track& t, bool pressed, uint32_t nowMs,
                               ButtonAction shortA, ButtonAction longA, ButtonAction doubleA) {
    bool wasDown = t.down;

    if (pressed) {
        // If the double-tap window already expired while released, emit the pending short now.
        if (t.pendingShort && (nowMs - t.releaseAt >= doubleMs_)) {
            enqueue(shortA);
            t.pendingShort = false;
        }

        if (!wasDown) {
            if (t.pendingShort && (nowMs - t.releaseAt < doubleMs_)) {
                // Second tap of a double-tap.
                t.pendingShort = false;
                t.pendingSecond = true;
                t.downAt = nowMs;
                t.longReported = false;
                t.alone = true;
            } else {
                // Fresh single press.
                t.downAt = nowMs;
                t.longReported = false;
                t.alone = true;
            }
        }

        if (t.alone && !t.longReported && (nowMs - t.downAt >= longMs_)) {
            t.longReported = true;
            t.pendingShort = false;
            t.pendingSecond = false;
            enqueue(longA);
        }
    } else {
        if (wasDown && t.alone && !t.longReported) {
            // Released before the long-press threshold.
            if (nowMs - t.downAt < longMs_) {
                if (t.pendingSecond) {
                    // Second short release -> double tap.
                    t.pendingSecond = false;
                    t.alone = false;
                    enqueue(doubleA);
                } else {
                    // First short release -> start the double-tap window.
                    t.pendingShort = true;
                    t.releaseAt = nowMs;
                    t.alone = false;
                }
            }
        }

        // If the double-tap window expires with no second tap, emit a short press.
        if (t.pendingShort && (nowMs - t.releaseAt >= doubleMs_)) {
            enqueue(shortA);
            t.pendingShort = false;
        }
    }

    t.down = pressed;
}
