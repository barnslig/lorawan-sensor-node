# LoRaWAN Sensor Node Firmware for Heltec WiFi LoRa 32 V2/V3

## Features

- LoRaWAN 1.1.0 Class A, via [RadioLib](https://github.com/jgromes/RadioLib)
- Deep sleep between uplinks
- OTAA join
- Sensor data is CayenneLPP encoded
- Default uplink interval: 20 minutes

## Supported sensors

Built-in support is provided for the following sensors:

- DS18B20 Temperature Sensor
- HC-SR04 Ultrasonic Range Sensor
- HTU21D(F) Relative Humidity and Temperature Sensor
- Internal battery voltage of the Heltec WiFi LoRa 32

Additional sensors can be added by implementing an [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) compatible interface.

All sensors listed at [Unified Sensor Drivers](https://github.com/adafruit/Adafruit_Sensor?tab=readme-ov-file#unified-sensor-drivers) should be compatible.

See [SensorCollection.cpp](src/SensorCollection.cpp) for a list of currently supported sensor types.

## Supported devices

Currently, the following boards are supported:

- Heltec WiFi LoRa 32 V2
- Heltec WiFi LoRa 32 V3

## Button

The node can be controlled by pushing the PRG button as follows:

- Press PRG for <1s: Show device info on the display.
- Press PRG for >1s and <5s: Immediately try to send an uplink message.
- Press PRG for >5s: Resets all node state and rejoins the network.

## Uplink message format

Uplink messages are CayenneLPP formatted. See [CayenneLPP by Electronic Cats - Library for Arduino](https://github.com/ElectronicCats/CayenneLPP).

## Development

The firmware is developed using [PlatformIO](https://platformio.org/) and the Arduino Environment.

Please refer to the PlatformIO manuals for details on development, compilation and flashing.
The project contains a VSCode configuration for PlatformIO.

## Provisioning a new sensor node

If you are using the things network, provision a new end device with the following settings:

- Input method: Enter end device specifics manually
- Frequency plan: Europe 863-870 Mhz
- LoRaWAN version: LoRaWAN Specification 1.1.0
- Regional Parameters version: RP001 Regional Parameters 1.1 revision A
- JoinEUI: `0000000000000000`
- DevEUI / AppKey / NwkKey / End device ID: Generate random

Then, proceed as follows:

1. Copy `include/lorawan-keys_example.h` to `include/lorawan-keys.h`
2. Copy activation information from TTN to `include/lorawan-keys.h`
3. Modify `includes/config.h` as needed
4. Rebuild and flash the firmware

## Related links

- [Kompost-Sensor: The previous firmware](https://github.com/barnslig/kompost-sensor)
- [Dragino SN50v3-LB/LS -- LoRaWAN Sensor Node User Manual](http://wiki.dragino.com/xwiki/bin/view/Main/User%20Manual%20for%20LoRaWAN%20End%20Nodes/SN50v3-LB/)

## License

See [LICENSE](./LICENSE).
