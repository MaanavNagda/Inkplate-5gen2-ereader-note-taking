#include "TextbookApp.h"
#include "../AppManager.h"
#include "Inkplate.h"

void TextbookApp::begin(AppManager& manager) {
    App::begin(manager);
    needsRender_ = true;
}

void TextbookApp::update(uint32_t dtMs) {
    (void)dtMs;
    if (needsRender_ && manager_) {
        render(manager_->display());
    }
}

void TextbookApp::render(Inkplate& display) {
    display.clearDisplay();
    display.fillScreen(7);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Textbook");

    display.setTextSize(1);
    display.setCursor(10, 60);
    display.println("WAKE short: previous page");
    display.setCursor(10, 80);
    display.println("IO   short: next page");
    display.setCursor(10, 100);
    display.println("IO   long:  menu / TOC");
    display.setCursor(10, 120);
    display.println("Both buttons: home");

    display.display();
    needsRender_ = false;
}

void TextbookApp::onButton(ButtonAction action) {
    if (!manager_) return;
    if (action == ButtonAction::IO_LONG) {
        manager_->switchTo(AppId::SELECTOR);
    }
}
