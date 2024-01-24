#ifndef LORAWAN_H_
#define LORAWAN_H_

#include <Arduino.h>
#include <CayenneLPP.h>
#include <RadioLib.h>

class LoRaWAN
{
public:
    LoRaWAN();

    void begin();

    void send(uint8_t fport, CayenneLPP *lpp);

    void sleep(uint16_t time_s);

private:
    Module mod;

#if defined(ARDUINO_heltec_wifi_lora_32_V3)
    SX1262 radio;
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
    SX1276 radio;
#endif

    LoRaWANNode node;
};

#endif // LORAWAN_H_
