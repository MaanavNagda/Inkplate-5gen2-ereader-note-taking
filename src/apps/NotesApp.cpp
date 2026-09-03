#include "NotesApp.h"
#include "../AppManager.h"
#include "Inkplate.h"

void NotesApp::begin(AppManager& manager) {
    App::begin(manager);
    needsRender_ = true;
}

void NotesApp::update(uint32_t dtMs) {
    (void)dtMs;
    if (needsRender_ && manager_) {
        render(manager_->display());
    }
}

void NotesApp::render(Inkplate& display) {
    display.clearDisplay();
    display.fillScreen(7);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Notes");

    display.setTextSize(1);
    display.setCursor(10, 60);
    display.println("BLE keyboard required");
    display.setCursor(10, 100);
    display.println("IO hold: home");

    display.display();
    needsRender_ = false;
}

void NotesApp::onButton(ButtonAction action) {
    if (!manager_) return;
    if (action == ButtonAction::IO_LONG) {
        manager_->switchTo(AppId::SELECTOR);
    }
}
