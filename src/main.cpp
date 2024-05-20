#include <Arduino.h>
#include <CayenneLPP.h>
#include <Wire.h>

#include "SensorCollection.h"
#include "LoRaWAN.h"
#include "PrgButton.h"

#include "config.h"
#include "lorawan-keys.h"

constexpr uint8_t kLoRaWANMaxPayloadSize = 51;
constexpr uint8_t kMaxSensors = 4;

#ifdef USE_DISPLAY
#include "Display.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, DISPLAY_RESET_PIN, DISPLAY_CLOCK_PIN, DISPLAY_DATA_PIN);

Display display(&u8g2);

DisplayState displayState = {
    .batteryVoltage = 0,
    .devEUI = devEUI,
    .isJoined = false,
    .fcntUp = 0};
#endif // USE_DISPLAY

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

#ifdef USE_BME280
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;
#endif // USE_BME280

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
LoRaWAN loraNode(&joinEUI, &devEUI, nwkKey, appKey);

#ifdef DEBUG_PRINT_SENSOR_VALUES
DynamicJsonDocument jsonBuffer(1024);
JsonObject root = jsonBuffer.to<JsonObject>();
#endif // DEBUG_PRINT_SENSOR_VALUES

void sleep()
{
  prgButton.sleep();
  loraNode.sleep(LORAWAN_UPLINK_INTERVAL);
}

void work()
{
#ifdef USE_BME280
  bme.takeForcedMeasurement();
#endif // USE_BME280

  lpp = sensors.update();

#ifdef DEBUG_PRINT_SENSOR_VALUES
  root.clear();
  lpp->decodeTTN(lpp->getBuffer(), lpp->getSize(), root);
  serializeJsonPretty(root, Serial);
  Serial.println();
#endif // DEBUG_PRINT_SENSOR_VALUES

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

#ifdef USE_DISPLAY
  display.begin();
#endif // USE_DISPLAY

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

#ifdef USE_BME280
  bme.begin(BME280_I2C_ADDRESS);

  /* Settings are according to BME280 datasheet recommended modes of operation
   * in section 3.5.1: weather monitoring.
   */
  bme.setSampling(
      Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1,
      Adafruit_BME280::SAMPLING_X1,
      Adafruit_BME280::SAMPLING_X1,
      Adafruit_BME280::FILTER_OFF);

  sensors.addSensor(bme.getTemperatureSensor(), BME280_TEMPERATURE_SENSOR_ID);
  sensors.addSensor(bme.getHumiditySensor(), BME280_HUMIDITY_SENSOR_ID);
  sensors.addSensor(bme.getPressureSensor(), BME280_PRESSURE_SENSOR_ID);
#endif // USE_BME280

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
    else
    {
      Serial.println(F("Display device info"));

#ifdef USE_HELTEC_BATTERY
      sensors_event_t event;
      batterySensor.getEvent(&event);
      displayState.batteryVoltage = event.voltage;
#endif // USE_HELTEC_BATTERY

      displayState.isJoined = loraNode.getNode()->isJoined();
      displayState.fcntUp = loraNode.getNode()->getFcntUp();

      display.print(&displayState);

      delay(10000);
    }

    sleep();
  }
}
