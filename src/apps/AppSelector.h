#ifndef APP_SELECTOR_H
#define APP_SELECTOR_H

#include "../App.h"

class AppSelector : public App {
public:
    AppId id() const override { return AppId::SELECTOR; }
    const char* name() const override { return "Home"; }
    void begin(AppManager& manager) override;
    void update(uint32_t dtMs) override;
    void render(Inkplate& display) override;
    void onButton(ButtonAction action) override;
private:
    bool needsRender_ = true;
};

#endif
