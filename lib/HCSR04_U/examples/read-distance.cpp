#include <Arduino.h>

#include "HCSR04_U.h"

constexpr int32_t kDistanceSensorId = 1;
constexpr uint8_t kDistanceSensorTriggerPin = 33;
constexpr uint8_t kDistancesensorEchoPin = 32;

HCSR04_U distanceSensor(kDistanceSensorId, kDistanceSensorTriggerPin, kDistancesensorEchoPin);

sensors_event_t event;

void setup()
{
    Serial.begin(9600);

    distanceSensor.begin();
}

void loop()
{
    distanceSensor.getEvent(&event);

    Serial.print("Distance: ");
    Serial.print(event.distance);
    Serial.println(" cm");

    delay(1000);
}