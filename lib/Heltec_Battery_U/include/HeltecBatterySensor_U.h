#ifndef HELTEC_BATTERY_SENSOR_U_H_
#define HELTEC_BATTERY_SENSOR_U_H_

#include <Adafruit_Sensor.h>

class HeltecBatterySensor_U : public Adafruit_Sensor
{
public:
    HeltecBatterySensor_U(int32_t sensor_id, uint8_t battery_pin, uint8_t drain_pin);

    void begin();

    bool getEvent(sensors_event_t *event);

    void getSensor(sensor_t *sensor);

private:
    int32_t sensor_id;
    uint8_t battery_pin;
    uint8_t drain_pin;
};

#endif // HELTEC_BATTERY_SENSOR_U_H_
