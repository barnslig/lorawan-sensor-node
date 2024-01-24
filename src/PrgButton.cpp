#include "PrgButton.h"

PrgButton::PrgButton() {}

void PrgButton::begin()
{
    rtc_gpio_pullup_en(GPIO_NUM_0);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
    {
        buttonPressedSince = millis();
    }
}

void PrgButton::sleep()
{
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
}

unsigned long PrgButton::update()
{
    buttonState = digitalRead(GPIO_NUM_0);

    if (buttonPressedSince == 0 && buttonState == 0)
    {
        buttonPressedSince = millis();
    }
    else if (buttonPressedSince > 0 && buttonState == 1)
    {
        unsigned long duration = millis() - buttonPressedSince;
        buttonPressedSince = 0;
        return duration;
    }

    return 0;
}
