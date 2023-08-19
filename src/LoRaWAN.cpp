#include "LoRaWAN.h"

// buffer to save current lmic state (size may be reduce)
RTC_DATA_ATTR uint8_t saveState[301];

void onLmicEvent(EventType ev)
{
    LoRaWAN *lora = LoRaWAN::getInstance();
    lora->onLmicEvent(ev);
}

LoRaWAN *LoRaWAN::_instance = nullptr;

LoRaWAN::LoRaWAN() : _doWorkCb(nullptr) {}

LoRaWAN *LoRaWAN::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new LoRaWAN();
    }
    return _instance;
}

void LoRaWAN::goToSleep(uint16_t deepSleepDurationSeconds)
{
    // save before going to deep sleep.
    auto store = StoringBuffer{saveState};
    LMIC.saveState(store);
    saveState[300] = 51;
    Serial.printf("State save len = %i\n", store.length());
    ESP.deepSleep(deepSleepDurationSeconds * 1000000ULL);
}

void LoRaWAN::doWork()
{
    Serial.println(F("do work"));

    // Execute external workload
    _doWorkCb();

    _nextSend = os_getTime() + OsDeltaTime::from_sec(_workIntervalSeconds);
}

void LoRaWAN::onLmicEvent(EventType ev)
{
    switch (ev)
    {
    case EventType::JOINING:
        Serial.println(F("EV_JOINING"));

        // Raise to SF12 for join tx
        // LMIC.setDrTx(0);

        break;

    case EventType::JOINED:
    {
        Serial.println(F("EV_JOINED"));

        // Disable Link Check Mode so that the sensor keeps transmitting
        // sensor data even if it has not received an ACK for some time.
        LMIC.setLinkCheckMode(_enableLinkCheckMode);

        // Reschedule work
        // os_clearCallback(&doWorkJob);
        // os_setCallback(&::doWorkJob, ::doWork);
        _nextSend = os_getTime();

        break;
    }

    case EventType::JOIN_FAILED:
    {
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    }

    case EventType::TXCOMPLETE:
    {
        Serial.println(F("EV_TXCOMPLETE"));

        if (LMIC.getTxRxFlags().test(TxRxStatus::ACK))
        {
            Serial.println(F("Received ack"));
        }

        if (LMIC.getDataLen())
        {
            Serial.printf("Received %d bytes of payload", LMIC.getDataLen());
        }

        _shouldSleep = true;

        break;
    }

    case EventType::RESET:
    {
        Serial.println(F("EV_RESET"));
        break;
    }

    case EventType::LINK_DEAD:
    {
        Serial.println(F("EV_LINK_DEAD"));
        break;
    }

    case EventType::LINK_ALIVE:
    {
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    }

    default:
    {
        Serial.println(F("Unknown event"));
        break;
    }
    }
}

void LoRaWAN::onDoWork(work_cb_t *cb)
{
    _doWorkCb = cb;
}

int8_t LoRaWAN::send(uint8_t fport, CayenneLPP *lpp)
{
    return LMIC.setTxData2(fport, lpp->getBuffer(), lpp->getSize(), 0);
}

void LoRaWAN::begin(bool isEnabled, bool enableLinkCheckMode, uint16_t workIntervalSeconds)
{
    _isEnabled = isEnabled;
    _enableLinkCheckMode = enableLinkCheckMode;
    _workIntervalSeconds = workIntervalSeconds;
    _shouldSleep = false;

    if (_isEnabled)
    {
        SPI.begin();

        // LMIC init
        os_init();
        LMIC.init();

        // Reset the MAC state. Session and pending data transfers will be discarded.
        LMIC.reset();
        LMIC.setEventCallBack(::onLmicEvent);
        SetupLmicKey<appEui, devEui, appKey>::setup(LMIC);

        // set clock error to allow good connection.
        LMIC.setClockError(MAX_CLOCK_ERROR * 3 / 100);

        if (saveState[300] == 51)
        {
            auto retrieve = RetrieveBuffer{saveState};
            LMIC.loadState(retrieve);
            saveState[300] = 0;
        }
    }

    // Start job (sending automatically starts OTAA too)
    _nextSend = os_getTime();
}

void LoRaWAN::loop()
{
    if (_isEnabled)
    {
        OsDeltaTime freeTimeBeforeNextCall = LMIC.run();

        if (freeTimeBeforeNextCall > OsDeltaTime::from_ms(10))
        {
            // we have more than 10 ms to do some work.
            // the test must be adapted from the time spend in other task
            if (_nextSend < os_getTime())
            {
                if (LMIC.getOpMode().test(OpState::TXRXPEND))
                {
                    {
                        Serial.println(F("OpState::TXRXPEND, not sending"));
                    }
                }

                else
                {
                    doWork();
                }
            }
            else
            {
                OsDeltaTime freeTimeBeforeSend = _nextSend - os_getTime();
                OsDeltaTime to_wait =
                    std::min(freeTimeBeforeNextCall, freeTimeBeforeSend);
                delay(to_wait.to_ms() / 2);
            }
        }
    }

    if (_shouldSleep)
    {
        goToSleep(_workIntervalSeconds);
    }
}