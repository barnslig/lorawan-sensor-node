#ifndef SENSOR_COLLECTION_H_
#define SENSOR_COLLECTION_H_

#include <Adafruit_Sensor.h>
#include <CayenneLPP.h>

/**
 * A collection of Adafruit Unified Sensors that can be updated simultaneously
 */
class SensorCollection
{
public:
    /**
     * Construct a new sensor collection
     *
     * @param size Maximum amount of sensors in this collection
     * @param lpp_size Maximum CayenneLPP payload size, in bytes
     */
    SensorCollection(uint8_t size, uint8_t lpp_size);

    /**
     * Add a sensor to the collection
     *
     * @param sensor Sensor to be added
     */
    void addSensor(Adafruit_Sensor *sensor);

    /**
     * Update data from all sensors in the collection
     *
     * @return CayenneLPP container with the updated data of all sensor
     */
    CayenneLPP *update();

private:
    /**
     * Maximum number of sensors in this collection
     */
    uint8_t _maxsize;

    /**
     * Index of the next non-used sensor pointer array slot
     */
    uint8_t _cursor;

    /**
     * Array with all sensor pointers of this collection
     */
    Adafruit_Sensor **_sensors;

    /**
     * Variable into which each sensor's data is read
     */
    sensors_event_t _event;

    /**
     * CayenneLPP container into which sensor data is serialized
     */
    CayenneLPP *_lpp;
};

#endif // SENSOR_COLLECTION_H_