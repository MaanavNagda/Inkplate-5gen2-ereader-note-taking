#include <Arduino.h>
#include "Inkplate.h"
#include <ButtonHandler.h>
#include "ButtonInput.h"
#include "AppManager.h"
#include "SleepManager.h"
#include "SerialFileReceiver.h"

Inkplate display(INKPLATE_3BIT);
ButtonHandler buttonHandler;
ButtonInput buttonInput;
AppManager appManager(display, buttonHandler, buttonInput);
SleepManager sleepManager(display, appManager);
SerialFileReceiver serialReceiver;

void setup() {
    Serial.setRxBufferSize(4096);
    Serial.begin(460800);
    Serial.println("Inkplate multi-app starting");

    display.begin();
    Serial.printf("einkOn after begin: %d\n", display.einkOn());
    display.clearDisplay();
    display.display();
    Serial.println("first display done");

    // If we woke from deep sleep, resume the previous app.
    bool resume = SleepManager::wasDeepSleep();

    appManager.begin();
    if (resume) {
        AppId last = static_cast<AppId>(SleepManager::lastAppIdRaw());
        Serial.printf("Resuming from deep sleep, app=%d\n", static_cast<int>(last));
        if (static_cast<int>(last) >= 0 && static_cast<int>(last) < 5) {
            appManager.switchTo(last);
        }
    }

    buttonInput.begin();
    buttonHandler.reset();
    sleepManager.begin();
    appManager.setSleepManager(sleepManager);

    Serial.println("Setup complete");
}

void loop() {
    uint32_t now = millis();
    bool serialBusy = serialReceiver.poll(display);
    if (!serialBusy) {
        bool active = appManager.update();
        sleepManager.update(now, active);
    } else {
        sleepManager.update(now, true);
    }
    delay(serialBusy ? 1 : 10);
}
