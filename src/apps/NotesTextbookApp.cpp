#include "NotesTextbookApp.h"
#include "../AppManager.h"
#include "Inkplate.h"

void NotesTextbookApp::begin(AppManager& manager) {
    App::begin(manager);
    needsRender_ = true;
}

void NotesTextbookApp::update(uint32_t dtMs) {
    (void)dtMs;
    if (needsRender_ && manager_) {
        render(manager_->display());
    }
}

void NotesTextbookApp::render(Inkplate& display) {
    display.clearDisplay();
    display.fillScreen(7);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Notes + Textbook");

    display.setTextSize(1);
    display.setCursor(10, 60);
    display.println("Top 80%: textbook page");
    display.setCursor(10, 80);
    display.println("Bottom 20%: notes sliver");
    display.setCursor(10, 120);
    display.println("IO hold: home");

    display.display();
    needsRender_ = false;
}

void NotesTextbookApp::onButton(ButtonAction action) {
    if (!manager_) return;
    if (action == ButtonAction::IO_LONG) {
        manager_->switchTo(AppId::SELECTOR);
    }
}
