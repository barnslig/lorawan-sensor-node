#include <Arduino.h>
#include <CayenneLPP.h>
#include <lmic.h>

#include "HCSR04_U.h"
#include "HeltecBatterySensor_U.h"
#include "SensorCollection.h"
#include "LoRaWAN.h"

constexpr bool kLoRaWANEnabled = true;
constexpr bool kLoRaWANEnableLinkCheckMode = false;
constexpr bool kLoRaWANEnableADR = true;
constexpr uint8_t kLoRaWANFPort = 10;
constexpr uint16_t kWorkIntervalSeconds = 45 * 60;
constexpr uint8_t kLoRaWANMaxPayloadSize = 51;

constexpr uint8_t kMaxSensors = 3;

constexpr int32_t kBatterySensorId = 1;
constexpr uint8_t kBatterySensorBatteryPin = 37;
constexpr uint8_t kBatterySensorDrainPin = 21;

constexpr int32_t kDistanceSensorId = 2;
constexpr uint8_t kDistanceSensorTriggerPin = 33;
constexpr uint8_t kDistancesensorEchoPin = 32;

HeltecBatterySensor_U batterySensor(kBatterySensorId, kBatterySensorBatteryPin, kBatterySensorDrainPin);

HCSR04_U distanceSensor(kDistanceSensorId, kDistanceSensorTriggerPin, kDistancesensorEchoPin);

SensorCollection sensors(kMaxSensors, kLoRaWANMaxPayloadSize);

CayenneLPP *lpp;

LoRaWAN *lora = LoRaWAN::getInstance();

void doWork()
{
  Serial.println("doWork");

  lpp = sensors.update();
  LMIC_setTxData2(kLoRaWANFPort, lpp->getBuffer(), lpp->getSize(), 0);
}

void setup()
{
  Serial.begin(115200);

  batterySensor.begin();
  distanceSensor.begin();

  sensors.addSensor(&batterySensor);
  sensors.addSensor(&distanceSensor);

  lora->onDoWork(&doWork);
  lora->begin(kLoRaWANEnabled, kLoRaWANEnableADR, kLoRaWANEnableLinkCheckMode, kWorkIntervalSeconds);
}

void loop()
{
  lora->loop();
}
