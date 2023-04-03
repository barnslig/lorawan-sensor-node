#ifndef HCSR04_U_H_
#define HCSR04_U_H_

#include <Adafruit_Sensor.h>

class HCSR04_U : public Adafruit_Sensor
{
public:
    HCSR04_U(int32_t sensor_id, uint8_t trigger_pin, uint8_t echo_pin);

    void begin();

    bool getEvent(sensors_event_t *);

    void getSensor(sensor_t *);

private:
    int32_t sensor_id;
    uint8_t trigger_pin;
    uint8_t echo_pin;
};

#endif // HCSR04_U_H_