#include "SensorCollection.h"

SensorCollection::SensorCollection(uint8_t size, uint8_t lpp_size) : _maxsize(size), _lpp(lpp_size)
{
    _sensorIds = (int32_t *)malloc(size * sizeof(int32_t));
    _sensors = (Adafruit_Sensor **)malloc(size * sizeof(Adafruit_Sensor *));
};

void SensorCollection::addSensor(Adafruit_Sensor *sensor, int32_t sensor_id)
{
    if (_cursor < _maxsize)
    {
        if (sensor_id > 0)
        {
            _sensorIds[_cursor] = sensor_id;
        }
        else
        {
            sensor_t sensor_details;
            sensor->getSensor(&sensor_details);
            _sensorIds[_cursor] = sensor_details.sensor_id;
        }

        _sensors[_cursor] = sensor;
        _cursor += 1;
    }
}

CayenneLPP *SensorCollection::update()
{
    _lpp.reset();

    for (uint8_t i = 0; i < _cursor; i += 1)
    {
        _sensors[i]->getEvent(&_event);

        switch (_event.type)
        {
        case SENSOR_TYPE_VOLTAGE:
        {
            _lpp.addAnalogInput(_sensorIds[i], _event.voltage);
            break;
        }
        case SENSOR_TYPE_PROXIMITY:
        {
            _lpp.addAnalogInput(_sensorIds[i], _event.distance);
            break;
        }
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        {
            _lpp.addTemperature(_sensorIds[i], _event.temperature);
            break;
        }
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        {
            _lpp.addRelativeHumidity(_sensorIds[i], _event.relative_humidity);
            break;
        }
        case SENSOR_TYPE_PRESSURE:
        {
            _lpp.addBarometricPressure(_sensorIds[i], _event.pressure);
            break;
        }
        }
    }

    return &_lpp;
}
