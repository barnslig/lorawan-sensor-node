#include "LoRaWAN.h"

static const u1_t PROGMEM DEVEUI[8] = {OTAA_DEVEUI};
static const u1_t PROGMEM APPEUI[8] = {OTAA_APPEUI};
static const u1_t PROGMEM APPKEY[16] = {OTAA_APPKEY};

// Below callbacks are used by LMIC for reading above values.
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

/**
 * Saves the LMIC structure during deep sleep
 */
RTC_DATA_ATTR lmic_t RTC_LMIC;

void printHex2(unsigned v)
{
    v &= 0xff;
    if (v < 16)
    {
        Serial.print('0');
    }
    Serial.print(v, HEX);
}

void printNetworkInfo()
{
    u4_t networkId = 0;
    devaddr_t deviceAddress = 0;
    u1_t networkSessionKey[16];
    u1_t applicationSessionKey[16];

    LMIC_getSessionKeys(&networkId, &deviceAddress, networkSessionKey, applicationSessionKey);

    Serial.print("netid: ");
    Serial.println(networkId, DEC);

    Serial.print("devaddr: ");
    Serial.println(deviceAddress, HEX);

    Serial.print("AppSKey: ");
    for (size_t i = 0; i < sizeof(uint8_t) * 16; ++i)
    {
        if (i != 0)
        {
            Serial.print("-");
        }
        printHex2(applicationSessionKey[i]);
    }

    Serial.println();

    Serial.print("NwkSKey: ");
    for (size_t i = 0; i < sizeof(uint8_t) * 16; ++i)
    {
        if (i != 0)
        {
            Serial.print("-");
        }
        printHex2(networkSessionKey[i]);
    }
    Serial.println();

    Serial.print(F("seqnoUp: "));
    Serial.println(LMIC.seqnoUp);

    Serial.print(F("seqnoDn: "));
    Serial.println(LMIC.seqnoDn);
}

LoRaWAN *LoRaWAN::_instance = nullptr;

void onLmicEvent(void *pUserData, ev_t ev)
{
    LoRaWAN *lora = LoRaWAN::getInstance();
    lora->onLmicEvent(pUserData, ev);
}

void doWork(osjob_t *j)
{
    LoRaWAN *lora = LoRaWAN::getInstance();
    lora->doWork(j);
}

LoRaWAN::LoRaWAN() : _doWorkCb(nullptr) {}

LoRaWAN *LoRaWAN::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new LoRaWAN();
    }
    return _instance;
}

void LoRaWAN::saveSession(uint16_t deepSleepDurationSeconds)
{
    RTC_LMIC = LMIC;

    // ESP32 can't track millis during DeepSleep and no option to advanced millis after DeepSleep.
    // Therefore reset DutyCyles

    unsigned long now = millis();

#if defined(CFG_LMIC_EU_like)
    for (int i = 0; i < MAX_BANDS; i++)
    {
        ostime_t correctedAvail = RTC_LMIC.bands[i].avail - sec2osticks(now / 1000.0 + deepSleepDurationSeconds);
        if (correctedAvail < 0)
        {
            correctedAvail = 0;
        }
        RTC_LMIC.bands[i].avail = correctedAvail;
    }

    RTC_LMIC.globalDutyAvail = RTC_LMIC.globalDutyAvail - sec2osticks(now / 1000.0 + deepSleepDurationSeconds);
    if (RTC_LMIC.globalDutyAvail < 0)
    {
        RTC_LMIC.globalDutyAvail = 0;
    }
#endif
}

void LoRaWAN::restoreSession()
{
    if (RTC_LMIC.seqnoUp != 0)
    {
        LMIC = RTC_LMIC;
        printNetworkInfo();
    }
}

void LoRaWAN::goToSleep(uint16_t deepSleepDurationSeconds)
{
    const bool timeCriticalJobs = os_queryTimeCriticalJobs(sec2osticks(deepSleepDurationSeconds));
    if (!_isEnabled || !timeCriticalJobs && !(LMIC.opmode & OP_TXRXPEND))
    {
        saveSession(deepSleepDurationSeconds);

        // esp_sleep_enable_ext0_wakeup(kPinPrgButton, 0);
        esp_sleep_enable_timer_wakeup(deepSleepDurationSeconds * 1000000ULL);

        Serial.println(F("enter deep sleep"));

        esp_deep_sleep_start();
    }
}

void LoRaWAN::doWork(osjob_t *j)
{
    // // Skip processWork if using OTAA and still joining.
    // if (LMIC.devaddr == 0)
    // {
    //     Serial.println(F("Uplink not scheduled because still joining."));
    //     return;
    // }

    // Check if there is not a current TX/RX job running
    // if (LMIC.opmode & OP_TXRXPEND)
    // {
    //     Serial.println(F("Uplink not scheduled because TxRx pending"));
    // }
    // else
    // {
    Serial.println(F("do work"));

    // Execute external workload
    _doWorkCb();

    // LMIC_setTxData2(kLoRaWANFPort, lpp.getBuffer(), lpp.getSize(), 0);
    // }
}

void LoRaWAN::onLmicEvent(void *pUserData, ev_t ev)
{
    switch (ev)
    {
    case EV_JOINING:
        Serial.println(lmicEventNames[ev]);

        // Raise SF for join tx
        LMIC_setDrTxpow(DR_SF12, 14);

        break;

    case EV_JOINED:
    {
        Serial.println(lmicEventNames[ev]);

        printNetworkInfo();

        // Disable Link Check Mode so that the sensor keeps transmitting
        // sensor data even if it has not received an ACK for some time.
        LMIC_setLinkCheckMode(_enableLinkCheckMode);

        // Reschedule work
        os_clearCallback(&doWorkJob);
        os_setCallback(&::doWorkJob, ::doWork);

        break;
    }

    case EV_TXCOMPLETE:
        Serial.println(lmicEventNames[ev]);

        _shouldSleep = true;

        break;

    case EV_TXSTART:
    case EV_RXCOMPLETE:
    case EV_JOIN_TXCOMPLETE:
    case EV_TXCANCELED:
    case EV_SCAN_TIMEOUT:
    case EV_BEACON_FOUND:
    case EV_BEACON_MISSED:
    case EV_BEACON_TRACKED:
    case EV_JOIN_FAILED:
    case EV_REJOIN_FAILED:
    case EV_LOST_TSYNC:
    case EV_RESET:
    case EV_LINK_DEAD:
    case EV_LINK_ALIVE:
        Serial.println(lmicEventNames[ev]);
        break;
    }
}

void LoRaWAN::onDoWork(work_cb_t *cb)
{
    _doWorkCb = cb;
}

void LoRaWAN::begin(bool isEnabled, bool enableADR, bool enableLinkCheckMode, uint16_t workIntervalSeconds)
{
    _isEnabled = isEnabled;
    _enableLinkCheckMode = enableLinkCheckMode;
    _workIntervalSeconds = workIntervalSeconds;
    _shouldSleep = false;

    if (_isEnabled)
    {
        // LMIC init using the computed target
        const lmic_pinmap *pPinMap = Arduino_LMIC::GetPinmap_ThisBoard();
        if (pPinMap == nullptr)
        {
            Serial.println(F("board not known to library; add pinmap or update getconfig_thisboard.cpp"));
        }

        // LMIC init
        os_init_ex(pPinMap);

        // Reset the MAC state. Session and pending data transfers will be discarded.
        LMIC_reset();

        // Enable or disable ADR (data rate adaptation).
        // Should be turned off if the device is not stationary (mobile).
        // 1 is on, 0 is off.
        LMIC_setAdrMode(enableADR);

        LMIC_registerEventCb(::onLmicEvent, nullptr);

        restoreSession();

        // LMIC_startJoining();

        // Schedule initial doWork job for immediate execution.
        os_setCallback(&::doWorkJob, ::doWork);
    }
}

void LoRaWAN::loop()
{
    if (_isEnabled)
    {
        os_runloop_once();
    }

    if (_shouldSleep)
    {
        goToSleep(_workIntervalSeconds);
    }
}