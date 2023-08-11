#ifndef HTU21DF_TEMPERATURE_U_H_
#define HTU21DF_TEMPERATURE_U_H_

#include <Adafruit_Sensor.h>
#include <Adafruit_HTU21DF.h>

class HTU21DF_Temperature_U : public Adafruit_Sensor
{
public:
    HTU21DF_Temperature_U(int32_t sensor_id, Adafruit_HTU21DF *sensor);

    void begin();

    bool getEvent(sensors_event_t *);

    void getSensor(sensor_t *);

private:
    int32_t sensor_id;

    Adafruit_HTU21DF *sensor;
};

#endif // HTU21DF_TEMPERATURE_U_H_
