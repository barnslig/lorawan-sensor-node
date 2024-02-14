#ifndef DALLESTEMPERATURE_U_H_
#define DALLESTEMPERATURE_U_H_

#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>

class DallasTemperature_U : public Adafruit_Sensor
{
public:
    DallasTemperature_U(int32_t sensor_id, uint8_t deviceIndex, DallasTemperature *sensor);

    void begin();

    bool getEvent(sensors_event_t *);

    void getSensor(sensor_t *);

private:
    int32_t sensor_id;

    uint8_t deviceIndex;

    DallasTemperature *sensor;

    DeviceAddress deviceAddress;
};

#endif // DALLESTEMPERATURE_U_H_
