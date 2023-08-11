#include "HTU21DF_Temperature_U.h"

HTU21DF_Temperature_U::HTU21DF_Temperature_U(int32_t sensor_id, Adafruit_HTU21DF *sensor) : sensor_id(sensor_id), sensor(sensor)
{
}

void HTU21DF_Temperature_U::begin()
{
    if (!sensor->begin())
    {
        Serial.println("Couldn't find sensor!");
    }
}

bool HTU21DF_Temperature_U::getEvent(sensors_event_t *event)
{
    float temperature = sensor->readTemperature();

    /* Clear the event */
    memset(event, 0, sizeof(sensors_event_t));

    event->version = sizeof(sensors_event_t);
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
    event->timestamp = 0;
    event->temperature = temperature;

    return true;
}

void HTU21DF_Temperature_U::getSensor(sensor_t *sensor)
{
    /* Clear the sensor_t object */
    memset(sensor, 0, sizeof(sensor_t));

    strncpy(sensor->name, "HTU21D TEMP", sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = 0;

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
    sensor->min_value = -40;
    sensor->max_value = 125;
    sensor->resolution = 0.1;
    sensor->min_delay = 0;
}
