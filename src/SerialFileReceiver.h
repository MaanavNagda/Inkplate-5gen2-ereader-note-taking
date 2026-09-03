#ifndef SERIAL_FILE_RECEIVER_H
#define SERIAL_FILE_RECEIVER_H

#include <cstdint>
#include <string>

#include "features/SdFat/SdFat.h"

class Inkplate;

class SerialFileReceiver {
public:
    SerialFileReceiver();

    // Call from loop. Returns true while a file transfer is in progress.
    bool poll(Inkplate& display);

    bool busy() const;
    uint32_t received() const;
    uint32_t totalSize() const;

private:
    enum class State { IDLE, WAIT_CHUNK, READ_CHUNK, DONE, ERROR };

    void reset();
    bool processCommand(Inkplate& display);
    bool processChunkHeader();
    void readChunkData();
    bool pathIsSafe(const std::string& path) const;
    void fail(const char* msg);

    State state_ = State::IDLE;
    std::string path_;
    uint32_t totalSize_ = 0;
    uint32_t received_ = 0;
    uint32_t chunkNeeded_ = 0;
    std::string lineBuf_;
    std::string error_;
    FsFile file_;
};

#endif
