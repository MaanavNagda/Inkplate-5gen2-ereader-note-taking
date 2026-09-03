#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include <cstdint>
#include <cstddef>
#include <array>
#include <memory>
#include "App.h"
#include "ButtonInput.h"
#include <ButtonHandler.h>

class Inkplate;
class SleepManager;

class AppManager {
public:
    AppManager(Inkplate& display, ButtonHandler& buttons, ButtonInput& input);

    void begin();
    bool update(); // returns true if a button was handled this tick

    void switchTo(AppId id);
    AppId currentAppId() const { return current_ ? current_->id() : AppId::SELECTOR; }

    void setSleepManager(SleepManager& sm);
    void enterDeepSleep();

    Inkplate& display() const { return display_; }
    ButtonHandler& buttons() const { return buttons_; }

private:
    Inkplate& display_;
    ButtonHandler& buttons_;
    ButtonInput& input_;
    std::array<std::unique_ptr<App>, 5> apps_;
    App* current_ = nullptr;
    SleepManager* sleepManager_ = nullptr;
    uint32_t lastUpdateMs_ = 0;
};

#endif
