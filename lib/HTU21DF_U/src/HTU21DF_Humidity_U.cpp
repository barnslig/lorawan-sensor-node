#include "HTU21DF_Humidity_U.h"

HTU21DF_Humidity_U::HTU21DF_Humidity_U(int32_t sensor_id, Adafruit_HTU21DF *sensor) : sensor_id(sensor_id), sensor(sensor)
{
}

void HTU21DF_Humidity_U::begin()
{
    if (!sensor->begin())
    {
        Serial.println("Couldn't find humidity sensor!");
    }
}

bool HTU21DF_Humidity_U::getEvent(sensors_event_t *event)
{
    float humidity = sensor->readHumidity();

    /* Clear the event */
    memset(event, 0, sizeof(sensors_event_t));

    event->version = sizeof(sensors_event_t);
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_RELATIVE_HUMIDITY;
    event->timestamp = 0;
    event->relative_humidity = humidity;

    return true;
}

void HTU21DF_Humidity_U::getSensor(sensor_t *sensor)
{
    /* Clear the sensor_t object */
    memset(sensor, 0, sizeof(sensor_t));

    strncpy(sensor->name, "HTU21D HUM", sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = 0;

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_RELATIVE_HUMIDITY;
    sensor->min_value = 0;
    sensor->max_value = 100;
    sensor->resolution = 0.1;
    sensor->min_delay = 0;
}
