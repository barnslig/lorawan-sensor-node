#ifndef PRG_BUTTON_H_
#define PRG_BUTTON_H_

#include <Arduino.h>
#include <driver/rtc_io.h>

/**
 * PRG button press
 */
class PrgButton
{
public:
    PrgButton();

    /**
     * Initialize the PRG button class
     *
     * Should be called in void setup() {}
     */
    void begin();

    /**
     * Prepare deep sleep by enabling wakeup via PRG button press
     */
    void sleep();

    /**
     * Check PRG button state and press duration
     *
     * Should be called in void loop() {}
     *
     * @return Button press duration, in ms. Zero if not pressed
     */
    unsigned long update();

private:
    unsigned long buttonPressedSince = 0;

    int buttonState = -1;
};

#endif // PRG_BUTTON_H_