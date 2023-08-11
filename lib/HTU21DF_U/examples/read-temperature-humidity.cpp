#include <Arduino.h>

#include "HTU21DF_Humidity_U.h"
#include "HTU21DF_Temperature_U.h"

constexpr int32_t kHumiditySensorId = 1;
constexpr int32_t kTemperatureSensorId = 2;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
HTU21DF_Humidity_U humiditySensor(kHumiditySensorId, &htu);
HTU21DF_Temperature_U temperatureSensor(kTemperatureSensorId, &htu);

sensors_event_t event;

void setup()
{
    Serial.begin(9600);

    // See https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/issues/62
    Wire.begin(4, 15);

    humiditySensor.begin();
    temperatureSensor.begin();
}

void loop()
{
    temperatureSensor.getEvent(&event);

    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" °C");

    humiditySensor.getEvent(&event);

    Serial.print("Relative Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println(" %");

    delay(1000);
}