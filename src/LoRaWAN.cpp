#include "LoRaWAN.h"
#include "lorawan-keys.h"

LoRaWAN::LoRaWAN() :
#if defined(ARDUINO_heltec_wifi_lora_32_V3)
                     // NSS pin: 8
                     // DIO1 pin: 14
                     // RESET pin: 12
                     // BUSY pin: 13
                     mod(8, 14, 12, 13),
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
                     // NSS pin: 18
                     // DIO0 pin: 26
                     // RESET pin: 14
                     // DIO1 pin: 35
                     mod(18, 26, 14, 35),
#endif
                     radio(&mod),
                     node(&radio, &EU868)
{
}

void LoRaWAN::begin()
{
    int state = radio.begin();
    if (state >= RADIOLIB_ERR_NONE)
    {
        Serial.println(F("LoRa radio initialization success!"));
    }
    else
    {
        Serial.print(F("LoRa radio initialization failed, code "));
        Serial.println(state);
        while (true)
            ;
    }
}

void LoRaWAN::join()
{
    if (joined)
        return;

    Serial.println(F("[LoRaWAN] Attempting over-the-air activation ... "));
    int state = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

    if (state >= RADIOLIB_ERR_NONE)
    {
        Serial.println(F("OTAA success!"));
        joined = true;
    }
    else
    {
        Serial.print(F("OTAA failed, code "));
        Serial.println(state);
        while (true)
            ;
    }
}

void LoRaWAN::send(uint8_t fport, CayenneLPP *lpp)
{
    join();

    // LoRaWAN downlinks can have 250 bytes at most with 1 extra byte for NULL
    size_t length = 0;
    uint8_t data[251];

    int state = node.sendReceive(lpp->getBuffer(), lpp->getSize(), fport, data, &length);

    if (state >= RADIOLIB_ERR_NONE)
    {
        Serial.println(F("received a downlink!"));

        // print data of the packet (if there are any)
        Serial.print(F("[LoRaWAN] Data:\t\t"));
        if (length > 0)
        {
            data[length] = '\0';
            String str = String((char *)data);
            Serial.println(str);
        }
        else
        {
            Serial.println(F("<MAC commands only>"));
        }

        // print RSSI (Received Signal Strength Indicator)
        Serial.print(F("[LoRaWAN] RSSI:\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print SNR (Signal-to-Noise Ratio)
        Serial.print(F("[LoRaWAN] SNR:\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        // print frequency error
        Serial.print(F("[LoRaWAN] Frequency error:\t"));
        Serial.print(radio.getFrequencyError());
        Serial.println(F(" Hz"));
    }
    else if (state == RADIOLIB_ERR_RX_TIMEOUT)
    {
        Serial.println(F("no downlink!"));
    }
    else
    {
        Serial.print(F("SEND failed, code "));
        Serial.println(state);
    }

    node.saveSession();
}

void LoRaWAN::sleep(uint16_t time_s)
{
    uint32_t minimumDelay = time_s * 1e3;
    uint32_t dutyCycleDelay = node.timeUntilUplink();
    uint32_t time_ms = max(dutyCycleDelay, minimumDelay);

    Serial.printf("Go to sleep for %u ms\n", time_ms);

    radio.sleep();
    ESP.deepSleep(time_ms * 1e3);
}

void LoRaWAN::reset()
{
    node.wipe();
    ESP.restart();
}