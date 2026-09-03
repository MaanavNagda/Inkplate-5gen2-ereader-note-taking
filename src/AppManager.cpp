#include "AppManager.h"
#include "Inkplate.h"
#include "SleepManager.h"
#include "apps/AppSelector.h"
#include "apps/EReaderApp.h"
#include "apps/TextbookApp.h"
#include "apps/NotesApp.h"
#include "apps/NotesTextbookApp.h"

AppManager::AppManager(Inkplate& display, ButtonHandler& buttons, ButtonInput& input)
    : display_(display), buttons_(buttons), input_(input) {}

void AppManager::begin() {
    apps_[static_cast<std::size_t>(AppId::SELECTOR)] = std::make_unique<AppSelector>();
    apps_[static_cast<std::size_t>(AppId::EREADER)] = std::make_unique<EReaderApp>();
    apps_[static_cast<std::size_t>(AppId::TEXTBOOK)] = std::make_unique<TextbookApp>();
    apps_[static_cast<std::size_t>(AppId::NOTES)] = std::make_unique<NotesApp>();
    apps_[static_cast<std::size_t>(AppId::NOTES_TEXTBOOK)] = std::make_unique<NotesTextbookApp>();

    switchTo(AppId::SELECTOR);
}

void AppManager::switchTo(AppId id) {
    if (current_) {
        // Future: onPause() hook.
    }
    current_ = apps_[static_cast<std::size_t>(id)].get();
    if (current_) {
        Serial.printf("Switching to app: %s\n", current_->name());
        // Everything except the deep-sleep screensaver runs in 1-bit for partial refresh.
        display_.selectDisplayMode(INKPLATE_1BIT);
        current_->begin(*this);
        // 1-bit mode: 0 is black, 7 is white.
        display_.setTextColor(0, 7);
        // Other apps are portrait; the App Selector will re-set landscape itself.
        display_.setRotation(0);
    }
}

bool AppManager::update() {
    uint32_t now = millis();
    uint32_t dt = now - lastUpdateMs_;
    lastUpdateMs_ = now;

    input_.update();
    buttons_.update(input_.wakePressed(), input_.ioPressed(), now);

    bool active = false;
    while (buttons_.hasAction()) {
        active = true;
        ButtonAction a = buttons_.getAction();
        if (current_) current_->onButton(a);
    }

    if (current_) current_->update(dt);
    return active;
}

void AppManager::setSleepManager(SleepManager& sm) {
    sleepManager_ = &sm;
}

void AppManager::enterDeepSleep() {
    if (sleepManager_) {
        sleepManager_->enterDeepSleep();
    }
}
