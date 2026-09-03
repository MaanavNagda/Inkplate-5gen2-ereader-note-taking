#ifndef APP_H
#define APP_H

#include <cstdint>
#include <ButtonHandler.h>

enum class AppId {
    SELECTOR,
    EREADER,
    TEXTBOOK,
    NOTES,
    NOTES_TEXTBOOK
};

class Inkplate;
class AppManager;

class App {
public:
    virtual ~App() = default;
    virtual AppId id() const = 0;
    virtual const char* name() const = 0;
    virtual void begin(AppManager& manager) { manager_ = &manager; }
    virtual void update(uint32_t dtMs) { (void)dtMs; }
    virtual void render(Inkplate& display) = 0;
    virtual void onButton(ButtonAction action) { (void)action; }
protected:
    AppManager* manager_ = nullptr;
};

#endif
