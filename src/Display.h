#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <U8g2lib.h>

struct DisplayState
{
    float batteryVoltage;
    uint64_t devEUI;
    bool isJoined;
    uint32_t fcntUp;
};

class Display
{
public:
    Display(U8G2 *u8g2);

    void begin();

    void print(DisplayState *state);

protected:
    U8G2 *u8g2;
};

#endif // DISPLAY_H_