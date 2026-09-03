#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <cstdint>
#include <cstddef>

enum class ButtonAction {
    NONE,
    WAKE_SHORT,
    WAKE_LONG,
    WAKE_DOUBLE,
    IO_SHORT,
    IO_LONG,
    IO_DOUBLE,
    BOTH
};

class ButtonHandler {
public:
    ButtonHandler(uint32_t longPressMs = 600, uint32_t bothWindowMs = 150, uint32_t doubleTapMs = 300);

    void reset();

    // Feed the current stable button states and the current time (ms).
    void update(bool wakePressed, bool ioPressed, uint32_t nowMs);

    bool hasAction() const;
    ButtonAction getAction();

    bool isWakePressed() const { return wake_; }
    bool isIoPressed() const { return io_; }

private:
    struct Track {
        bool down = false;
        bool alone = false;
        bool longReported = false;
        bool pendingShort = false;
        bool pendingSecond = false;
        uint32_t downAt = 0;
        uint32_t releaseAt = 0;
    };

    static constexpr std::size_t QUEUE_SIZE = 8;

    void enqueue(ButtonAction action);
    void processOne(Track& t, bool pressed, uint32_t nowMs,
                    ButtonAction shortA, ButtonAction longA, ButtonAction doubleA);

    uint32_t longMs_;
    uint32_t bothMs_;
    uint32_t doubleMs_;

    bool wake_;
    bool io_;
    Track wakeTrack_;
    Track ioTrack_;
    bool bothActive_;
    bool bothReported_;
    uint32_t bothStart_;

    ButtonAction queue_[QUEUE_SIZE];
    std::size_t head_;
    std::size_t tail_;
};

#endif
