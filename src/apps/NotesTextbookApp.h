#ifndef NOTES_TEXTBOOK_APP_H
#define NOTES_TEXTBOOK_APP_H

#include "../App.h"

class NotesTextbookApp : public App {
public:
    AppId id() const override { return AppId::NOTES_TEXTBOOK; }
    const char* name() const override { return "Notes + Textbook"; }
    void begin(AppManager& manager) override;
    void update(uint32_t dtMs) override;
    void render(Inkplate& display) override;
    void onButton(ButtonAction action) override;
private:
    bool needsRender_ = true;
};

#endif
