#include <Arduino.h>

#include "HeltecBatterySensor_U.h"

constexpr int32_t kBatterySensorId = 1;
constexpr uint8_t kBatterySensorBatteryPin = 37;
constexpr uint8_t kBatterySensorDrainPin = 21;

HeltecBatterySensor_U batterySensor(kBatterySensorId, kBatterySensorBatteryPin, kBatterySensorDrainPin);

sensors_event_t event;

void setup()
{
    Serial.begin(9600);

    batterySensor.begin();
}

void loop()
{
    batterySensor.getEvent(&event);

    Serial.print("Battery: ");
    Serial.print(event.voltage);
    Serial.println(" V");

    delay(1000);
}