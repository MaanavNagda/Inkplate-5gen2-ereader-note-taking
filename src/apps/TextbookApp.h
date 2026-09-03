#ifndef TEXTBOOK_APP_H
#define TEXTBOOK_APP_H

#include "../App.h"

class TextbookApp : public App {
public:
    AppId id() const override { return AppId::TEXTBOOK; }
    const char* name() const override { return "Textbook"; }
    void begin(AppManager& manager) override;
    void update(uint32_t dtMs) override;
    void render(Inkplate& display) override;
    void onButton(ButtonAction action) override;
private:
    bool needsRender_ = true;
};

#endif
