#ifndef CONFIG_H_
#define CONFIG_H_

#define LORAWAN_F_PORT 10               // Uplink fport
#define LORAWAN_UPLINK_INTERVAL 20 * 60 // Work interval, in seconds

#define USE_HELTEC_BATTERY
#define HELTEC_BATTERY_SENSOR_ID 1
#ifdef ARDUINO_heltec_wifi_lora_32_V3
#define HELTEC_BATTERY_BATTERY_PIN 1
#define HELTEC_BATTERY_DRAIN_PIN 37
#endif // ARDUINO_heltec_wifi_lora_32_V3
#ifdef ARDUINO_HELTEC_WIFI_LORA_32_V2
#define HELTEC_BATTERY_BATTERY_PIN 37
#define HELTEC_BATTERY_DRAIN_PIN 21
#endif // ARDUINO_HELTEC_WIFI_LORA_32_V2

#define USE_HTU
#define HTU_HUMIDITY_SENSOR_ID 2
#define HTU_TEMPERATURE_SENSOR_ID 3

// #define USE_SR04
// #define SR04_SENSOR_ID 4
// #define SR04_TRIGGER_PIN 33
// #define SR04_ECHO_PIN 32

#endif // CONFIG_H_
