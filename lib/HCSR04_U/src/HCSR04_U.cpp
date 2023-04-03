#include "HCSR04_U.h"

HCSR04_U::HCSR04_U(int32_t sensor_id, uint8_t trigger_pin, uint8_t echo_pin) : sensor_id(sensor_id),
                                                                               trigger_pin(trigger_pin),
                                                                               echo_pin(echo_pin)
{
}

void HCSR04_U::begin()
{
    pinMode(trigger_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
}

bool HCSR04_U::getEvent(sensors_event_t *event)
{
    digitalWrite(trigger_pin, LOW);

    delayMicroseconds(5);

    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    long duration = pulseIn(echo_pin, HIGH);
    float distance = duration * 0.034 / 2;

    /* Clear the event */
    memset(event, 0, sizeof(sensors_event_t));

    event->version = sizeof(sensors_event_t);
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_PROXIMITY;
    event->timestamp = 0;
    event->distance = distance;

    return true;
}

void HCSR04_U::getSensor(sensor_t *sensor)
{
    /* Clear the sensor_t object */
    memset(sensor, 0, sizeof(sensor_t));

    strncpy(sensor->name, "HC-SR04", sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = 0;

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_PROXIMITY;
    sensor->min_value = 19;
    sensor->max_value = 400;
    sensor->resolution = 0.1;
    sensor->min_delay = 0;
}
