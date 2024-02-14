#include <Arduino.h>
#include <CayenneLPP.h>

#include "SensorCollection.h"
#include "LoRaWAN.h"
#include "PrgButton.h"

#include "config.h"

constexpr uint8_t kLoRaWANMaxPayloadSize = 51;
constexpr uint8_t kMaxSensors = 3;

#ifdef USE_HELTEC_BATTERY
#include "HeltecBatterySensor_U.h"

HeltecBatterySensor_U batterySensor(HELTEC_BATTERY_SENSOR_ID, HELTEC_BATTERY_BATTERY_PIN, HELTEC_BATTERY_DRAIN_PIN);
#endif // USE_HELTEC_BATTERY

#ifdef USE_HTU
#include "HTU21DF_Humidity_U.h"
#include "HTU21DF_Temperature_U.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
HTU21DF_Humidity_U humiditySensor(HTU_HUMIDITY_SENSOR_ID, &htu);
HTU21DF_Temperature_U temperatureSensor(HTU_TEMPERATURE_SENSOR_ID, &htu);
#endif // USE_HTU

#ifdef USE_SR04
#include "HCSR04_U.h"

HCSR04_U distanceSensor(SR04_SENSOR_ID, SR04_TRIGGER_PIN, SR04_ECHO_PIN);
#endif // USE_SR04

#ifdef USE_DS18
#include "DallasTemperature_U.h"

OneWire oneWire(DS18_ONEWIRE_PIN);
DallasTemperature tempSensors(&oneWire);

DallasTemperature_U *temperatureSensors[DS18_SENSOR_NUM];
#endif // USE_DS18

SensorCollection sensors(kMaxSensors, kLoRaWANMaxPayloadSize);

CayenneLPP *lpp;

PrgButton prgButton;
LoRaWAN loraNode;

void sleep()
{
  prgButton.sleep();
  loraNode.sleep(LORAWAN_UPLINK_INTERVAL);
}

void work()
{
  lpp = sensors.update();

  loraNode.send(LORAWAN_F_PORT, lpp);

  sleep();
}

void setup()
{
  Serial.begin(115200);

#if defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
  // See https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62
  Wire.begin(4, 15);
#endif

#ifdef USE_HELTEC_BATTERY
  batterySensor.begin();
  sensors.addSensor(&batterySensor);
#endif // USE_HELTEC_BATTERY

#ifdef USE_HTU
  humiditySensor.begin();
  temperatureSensor.begin();
  sensors.addSensor(&humiditySensor);
  sensors.addSensor(&temperatureSensor);
#endif // USE_HTU

#ifdef USE_SR04
  distanceSensor.begin();
  sensors.addSensor(&distanceSensor);
#endif // USE_SR04

#ifdef USE_DS18
  tempSensors.begin();

  for (uint8_t i = 0; i < DS18_SENSOR_NUM; i += 1)
  {
    temperatureSensors[i] = new DallasTemperature_U(DS18_SENSOR_ID + i, i, &tempSensors);
    temperatureSensors[i]->begin();
    sensors.addSensor(temperatureSensors[i]);
  }
#endif // USE_DS18

  prgButton.begin();
  loraNode.begin();

  esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
  if (wakeupCause == ESP_SLEEP_WAKEUP_TIMER || wakeupCause == ESP_SLEEP_WAKEUP_UNDEFINED)
  {
    // Start an uplink after wakeup from regular reset or timer
    work();
  }
}

void loop()
{
  unsigned long prgBtnDuration = prgButton.update();
  if (prgBtnDuration > 0)
  {
    Serial.printf("Button pressed for: %lu ms\n", prgBtnDuration);

    if (prgBtnDuration > 5000)
    {
      Serial.println(F("Reset device"));
      loraNode.reset();
    }
    else if (prgBtnDuration > 1000)
    {
      Serial.println(F("Immediate uplink"));
      work();
    }

    sleep();
  }
}
