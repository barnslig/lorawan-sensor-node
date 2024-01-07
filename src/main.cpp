#include <Arduino.h>
#include <CayenneLPP.h>

#include "HeltecBatterySensor_U.h"
#include "HTU21DF_Humidity_U.h"
#include "HTU21DF_Temperature_U.h"
#include "SensorCollection.h"
#include "LoRaWAN.h"

constexpr bool kLoRaWANEnabled = true;
constexpr bool kLoRaWANEnableLinkCheckMode = true;
constexpr uint8_t kLoRaWANFPort = 10;
constexpr uint16_t kWorkIntervalSeconds = 5 * 60;
constexpr uint8_t kLoRaWANMaxPayloadSize = 51;

constexpr uint8_t kMaxSensors = 3;

constexpr int32_t kBatterySensorId = 1;
#if defined(ARDUINO_heltec_wifi_lora_32_V3)
constexpr uint8_t kBatterySensorBatteryPin = 1;
constexpr uint8_t kBatterySensorDrainPin = 37;
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
constexpr uint8_t kBatterySensorBatteryPin = 37;
constexpr uint8_t kBatterySensorDrainPin = 21;
#endif

constexpr int32_t kHumiditySensorId = 3;
constexpr int32_t kTemperatureSensorId = 4;

HeltecBatterySensor_U batterySensor(kBatterySensorId, kBatterySensorBatteryPin, kBatterySensorDrainPin);

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
HTU21DF_Humidity_U humiditySensor(kHumiditySensorId, &htu);
HTU21DF_Temperature_U temperatureSensor(kTemperatureSensorId, &htu);

SensorCollection sensors(kMaxSensors, kLoRaWANMaxPayloadSize);

CayenneLPP *lpp;

LoRaWAN loraNode;

void setup()
{
  Serial.begin(115200);

#if defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
  // See https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62
  Wire.begin(4, 15);
#endif

  batterySensor.begin();
  humiditySensor.begin();
  temperatureSensor.begin();

  sensors.addSensor(&batterySensor);
  sensors.addSensor(&humiditySensor);
  sensors.addSensor(&temperatureSensor);

  loraNode.begin();

  lpp = sensors.update();
  loraNode.send(kLoRaWANFPort, lpp);

  Serial.println("Go to sleep");
  ESP.deepSleep(kWorkIntervalSeconds * 1e6);
}

void loop()
{
}
