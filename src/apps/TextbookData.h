#ifndef TEXTBOOK_DATA_H
#define TEXTBOOK_DATA_H

#include <cstddef>

namespace textbook {

struct Entry {
    const char* title;
    size_t pageCount;
    const char* pathPrefix;
};

extern const Entry entries[];
extern const size_t entryCount;

} // namespace textbook

#endif // TEXTBOOK_DATA_H
