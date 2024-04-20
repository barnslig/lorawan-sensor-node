#ifndef CONFIG_H_
#define CONFIG_H_

#define DEBUG_PRINT_SENSOR_VALUES

#define LORAWAN_F_PORT 10               // Uplink fport
#define LORAWAN_UPLINK_INTERVAL 20 * 60 // Work interval, in seconds

#define USE_DISPLAY
#define DISPLAY_RESET_PIN 16
#define DISPLAY_CLOCK_PIN 15
#define DISPLAY_DATA_PIN 4

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

// #define USE_DS18
// #define DS18_ONEWIRE_PIN 13
// #define DS18_SENSOR_ID 5
// #define DS18_SENSOR_NUM 2

#endif // CONFIG_H_
