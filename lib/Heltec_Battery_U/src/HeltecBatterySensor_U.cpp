#include "HeltecBatterySensor_U.h"

HeltecBatterySensor_U::HeltecBatterySensor_U(int32_t sensor_id, uint8_t battery_pin, uint8_t drain_pin) : sensor_id(sensor_id),
                                                                                                          battery_pin(battery_pin),
                                                                                                          drain_pin(drain_pin)
{
}

void HeltecBatterySensor_U::begin()
{
    pinMode(drain_pin, OUTPUT);
}

bool HeltecBatterySensor_U::getEvent(sensors_event_t *event)
{
    digitalWrite(drain_pin, LOW);

    delay(10);

    uint32_t mV = analogReadMilliVolts(battery_pin);
    float corr_V = mV * (220 + 100) / 100 * 0.001;

    digitalWrite(drain_pin, HIGH);

    /* Clear the event */
    memset(event, 0, sizeof(sensors_event_t));

    event->version = sizeof(sensors_event_t);
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_VOLTAGE;
    event->timestamp = millis();
    event->voltage = corr_V;

    return true;
}

void HeltecBatterySensor_U::getSensor(sensor_t *sensor)
{
    /* Clear the sensor_t object */
    memset(sensor, 0, sizeof(sensor_t));

    strncpy(sensor->name, "HeltecBat", sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = 0;

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_VOLTAGE;
    sensor->min_value = 3.2;
    sensor->max_value = 4.2;
    sensor->resolution = 0.01;
    sensor->min_delay = 0;
}