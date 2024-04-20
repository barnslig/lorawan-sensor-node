#include "LoRaWAN.h"

// utilities & vars to support ESP32 deep-sleep. The RTC_DATA_ATTR attribute
// puts these in to the RTC memory which is preserved during deep-sleep
RTC_DATA_ATTR uint16_t bootCount = 1;
RTC_DATA_ATTR uint16_t bootCountSinceUnsuccessfulJoin = 0;
RTC_DATA_ATTR uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];

LoRaWAN::LoRaWAN(
    uint64_t *joinEUI,
    uint64_t *devEUI,
    uint8_t *nwkKey,
    uint8_t *appKey) : joinEUI(joinEUI),
                       devEUI(devEUI),
                       nwkKey(nwkKey),
                       appKey(appKey),
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
    Serial.print(F("Boot count: "));
    Serial.println(bootCount++);

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

    store.begin("radiolib");

    Serial.println(F("Recalling LoRaWAN nonces & session"));

    if (store.isKey("nonces"))
    {
        uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
        store.getBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);

        state = node.setBufferNonces(buffer);
        if (state != RADIOLIB_ERR_NONE)
        {
            Serial.print(F("Restoring nonces buffer failed"));
            Serial.println(state);
        }
    }

    state = node.setBufferSession(LWsession);

    // if we have booted at least once we should have a session to restore, so report any failure
    // otherwise no point saying there's been a failure when it was bound to fail with an empty
    // LWsession var. At this point, bootCount has already been incremented, hence the > 2
    if ((state != RADIOLIB_ERR_NONE) && (bootCount > 2))
    {
        Serial.print(F("Restoring session buffer failed"));
        Serial.println(state);
    }
}

void LoRaWAN::join()
{
    if (joined)
        return;

    int state = node.beginOTAA(*joinEUI, *devEUI, nwkKey, appKey, false, 0);

    if ((state != RADIOLIB_ERR_NONE) && (bootCount > 2))
    {
        Serial.print(F("Restore session failed "));
        Serial.println(state);
    }

    while (state != RADIOLIB_ERR_NONE)
    {
        Serial.println(F("[LoRaWAN] Attempting over-the-air activation. Previous error code: "));
        Serial.println(state);
        state = node.beginOTAA(*joinEUI, *devEUI, nwkKey, appKey, true, 0);

        if (state < RADIOLIB_ERR_NONE)
        {
            Serial.print(F("Join failed: "));
            Serial.println(state);

            // how long to wait before join attempts. This is an interim solution pending
            // implementation of TS001 LoRaWAN Specification section #7 - this doc applies to v1.0.4 & v1.1
            // it sleeps for longer & longer durations to give time for any gateway issues to resolve
            // or whatever is interfering with the device <-> gateway airwaves.
            uint32_t sleepForSeconds = min((bootCountSinceUnsuccessfulJoin++ + 1UL) * 60UL, 3UL * 60UL);
            Serial.print(F("Boots since unsuccessful join: "));
            Serial.println(bootCountSinceUnsuccessfulJoin);
            Serial.print(F("Retrying join in "));
            Serial.print(sleepForSeconds);
            Serial.println(F(" seconds"));

            sleep(sleepForSeconds);
        }
        else
        {
            Serial.println(F("Joined"));

            Serial.println(F("Saving nonces to flash"));
            uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
            uint8_t *persist = node.getBufferNonces();
            memcpy(buffer, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
            store.putBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);

            // we'll save the session after the uplink

            // reset the failed join count
            bootCountSinceUnsuccessfulJoin = 0;

            delay(1000); // hold off off hitting the airwaves again too soon - an issue in the US
        }

        store.end();
    }
}

void LoRaWAN::send(uint8_t fport, CayenneLPP *lpp)
{
    join();

    // LoRaWAN downlinks can have 250 bytes at most with 1 extra byte for NULL
    size_t length = 0;
    uint8_t data[251];

    int state = node.sendReceive(lpp->getBuffer(), lpp->getSize(), fport, data, &length);

    Serial.printf("fcnt: %u\n", node.getFcntUp());

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

    // now save session to RTC memory
    uint8_t *persist = node.getBufferSession();
    memcpy(LWsession, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
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
    bootCount = 1;
    bootCountSinceUnsuccessfulJoin = 0;
    memset(LWsession, 0, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

    ESP.restart();
}

LoRaWANNode *LoRaWAN::getNode()
{
    return &node;
}