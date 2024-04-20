#include "Display.h"
#include "config.h"

Display::Display(U8G2 *u8g2) : u8g2(u8g2) {}

void Display::begin()
{
    u8g2->begin();
}

void Display::print(DisplayState *state)
{
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_siji_t_6x10); // 12x12

    u8g2->setCursor(0, 12);
    u8g2->print(F("eui-"));
    u8g2->print(state->devEUI, HEX);

    u8g2->setCursor(0, 28);
    u8g2->print(F("Joined?: "));
    u8g2->print(state->isJoined ? F("Yes") : F("No"));

    u8g2->setCursor(0, 44);
    u8g2->print(F("FCntUp: "));
    u8g2->print(state->fcntUp);

#ifdef USE_HELTEC_BATTERY
    u8g2->setCursor(0, 60);
    u8g2->print(F("Battery: "));
    u8g2->print(state->batteryVoltage);
    u8g2->print(F(" V"));
#endif // USE_HELTEC_BATTERY

    u8g2->sendBuffer();
}
