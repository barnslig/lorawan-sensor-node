#include <Arduino.h>

#include "DallasTemperature_U.h"

/**
 * Pin at which the DS18B20 data pin is connected
 */
constexpr uint8_t kPinTempSensors = 13;

constexpr int32_t kTemperatureSensorId1 = 1;
constexpr int32_t kTemperatureSensorId2 = 2;

OneWire oneWire(kPinTempSensors);

DallasTemperature tempSensors(&oneWire);

DallasTemperature_U temperatureSensor1(kTemperatureSensorId1, 0, &tempSensors);
DallasTemperature_U temperatureSensor2(kTemperatureSensorId2, 1, &tempSensors);

sensors_event_t event;

void setup()
{
    Serial.begin(9600);

    tempSensors.begin();
    temperatureSensor1.begin();
    temperatureSensor2.begin();
}

void loop()
{
    temperatureSensor1.getEvent(&event);

    Serial.print("Temperature 1: ");
    Serial.print(event.temperature);
    Serial.println(" °C");

    temperatureSensor2.getEvent(&event);

    Serial.print("Temperature 2: ");
    Serial.print(event.temperature);
    Serial.println(" °C");
}