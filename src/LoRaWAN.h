#ifndef LORAWAN_H_
#define LORAWAN_H_

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <arduino_lmic_hal_boards.h>

#include "lorawan-keys.h"

static const char *const lmicEventNames[] = {LMIC_EVENT_NAME_TABLE__INIT};

static osjob_t doWorkJob;

typedef void work_cb_t();

class LoRaWAN
{
public:
    LoRaWAN(LoRaWAN &other) = delete;
    void operator=(const LoRaWAN &) = delete;

    static LoRaWAN *getInstance();

    /**
     * Copy the LMIC state to ESP32 RTC RAM
     *
     * @param deepSleepDuration Deep sleep duration, in seconds
     * @see https://github.com/JackGruber/ESP32-LMIC-DeepSleep-example
     */
    void saveSession(uint16_t deepSleepDurationSeconds);

    /**
     * Restore the LMIC state from ESP32 RTC RAM
     */
    void restoreSession();

    /**
     * Save the LMIC session and put the ESP32 into deep sleep
     */
    void goToSleep(uint16_t deepSleepDurationSeconds);

    void begin(bool isEnabled, bool enableADR, bool enableLinkCheckMode, uint16_t workIntervalSeconds);

    void loop();

    void onLmicEvent(void *pUserData, ev_t ev);

    void doWork(osjob_t *j);

    void onDoWork(work_cb_t *cb);

protected:
private:
    LoRaWAN();

    static LoRaWAN *_instance;

    bool _isEnabled;

    bool _enableLinkCheckMode;

    bool _shouldSleep;

    uint16_t _workIntervalSeconds;

    work_cb_t *_doWorkCb;
};

#endif // LORAWAN_H_
