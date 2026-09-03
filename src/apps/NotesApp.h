#ifndef NOTES_APP_H
#define NOTES_APP_H

#include "../App.h"

class NotesApp : public App {
public:
    AppId id() const override { return AppId::NOTES; }
    const char* name() const override { return "Notes"; }
    void begin(AppManager& manager) override;
    void update(uint32_t dtMs) override;
    void render(Inkplate& display) override;
    void onButton(ButtonAction action) override;
private:
    bool needsRender_ = true;
};

#endif
