#include "SensorCollection.h"

SensorCollection::SensorCollection(uint8_t size, uint8_t lpp_size) : _maxsize(size), _lpp(lpp_size)
{
    _sensors = (Adafruit_Sensor **)malloc(size * sizeof(Adafruit_Sensor *));
};

void SensorCollection::addSensor(Adafruit_Sensor *sensor)
{
    if (_cursor < _maxsize)
    {
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
            _lpp.addAnalogInput(_event.sensor_id, _event.voltage);
            break;
        }
        case SENSOR_TYPE_PROXIMITY:
        {
            _lpp.addAnalogInput(_event.sensor_id, _event.distance);
            break;
        }
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        {
            _lpp.addTemperature(_event.sensor_id, _event.temperature);
            break;
        }
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        {
            _lpp.addRelativeHumidity(_event.sensor_id, _event.relative_humidity);
            break;
        }
        }
    }

    return &_lpp;
}
