#include "DallasTemperature_U.h"

DallasTemperature_U::DallasTemperature_U(int32_t sensor_id, uint8_t deviceIndex, DallasTemperature *sensor) : sensor_id(sensor_id), deviceIndex(deviceIndex), sensor(sensor)
{
}

void DallasTemperature_U::begin()
{
    if (!sensor->getAddress(deviceAddress, deviceIndex))
    {
        Serial.println("Couldn't find temperature sensor!");
    }
}

bool DallasTemperature_U::getEvent(sensors_event_t *event)
{
    sensor->requestTemperatures();
    float temperature = sensor->getTempC(deviceAddress);

    /* Clear the event */
    memset(event, 0, sizeof(sensors_event_t));

    event->version = sizeof(sensors_event_t);
    event->sensor_id = sensor_id;
    event->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
    event->timestamp = 0;
    event->temperature = temperature;

    return true;
}

void DallasTemperature_U::getSensor(sensor_t *sensor)
{
    /* Clear the sensor_t object */
    memset(sensor, 0, sizeof(sensor_t));

    strncpy(sensor->name, "DS18 Temp", sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = 0;

    sensor->version = 1;
    sensor->sensor_id = sensor_id;
    sensor->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
    sensor->min_value = -55;
    sensor->max_value = 125;
    sensor->resolution = 0.1;
    sensor->min_delay = 0;
}
