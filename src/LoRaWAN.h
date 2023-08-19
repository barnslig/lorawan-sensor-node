#ifndef LORAWAN_H_
#define LORAWAN_H_

#include <Arduino.h>
#include <SPI.h>
#include <CayenneLPP.h>

#include <hal/hal_io.h>
#include <hal/print_debug.h>
#include <keyhandler.h>
#include <lmic.h>

#include "lorawan-keys.h"

#if defined(ARDUINO_heltec_wifi_lora_32_V3)
constexpr lmic_pinmap lmic_pins = {
    .nss = 8,
    .prepare_antenna_tx = nullptr,
    .rst = 12,
    .dio = {/* busy */ 13, /* DIO1 */ 14},
};
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
constexpr lmic_pinmap lmic_pins = {
    .nss = 18,
    .prepare_antenna_tx = nullptr,
    .rst = 14,
    .dio = {DIO0, DIO1},
};
#endif

typedef void work_cb_t();

class LoRaWAN
{
public:
    LoRaWAN(LoRaWAN &other) = delete;
    void operator=(const LoRaWAN &) = delete;

    static LoRaWAN *getInstance();

    void begin(bool isEnabled, bool enableLinkCheckMode, uint16_t workIntervalSeconds);

    void loop();

    void onLmicEvent(EventType ev);

    void onDoWork(work_cb_t *cb);

    int8_t send(uint8_t fport, CayenneLPP *lpp);

protected:
private:
    LoRaWAN();

    /**
     * Save the LMIC session and put the ESP32 into deep sleep
     */
    void goToSleep(uint16_t deepSleepDurationSeconds);

    void doWork();

    static LoRaWAN *_instance;

    bool _isEnabled;

    bool _enableLinkCheckMode;

    bool _shouldSleep;

    uint16_t _workIntervalSeconds;

    work_cb_t *_doWorkCb;

#if defined(ARDUINO_heltec_wifi_lora_32_V3)
    RadioSx1262 radio{lmic_pins, ImageCalibrationBand::band_863_870};
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
    RadioSx1276 radio{lmic_pins};
#endif

    LmicEu868 LMIC{radio};

    OsTime _nextSend;
};

#endif // LORAWAN_H_
