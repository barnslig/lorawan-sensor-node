/* lorawan-keys.h defines LoRaWAN keys needed by OTAA activation
 *
 * It is essential that each key is specified in the correct format.
 * lsb: least-significant-byte first, msb: most-significant-byte first.
 *
 * For security reasons all files in the keyfiles folder (except file
 * lorawan-keys_example.h) are excluded from the Git(Hub) repository.
 * Also excluded are all files matching the pattern *lorawan-keys.h.
 * This way they cannot be accidentally committed to a public repository.
 */
#ifndef LORAWAN_KEYS_H_
#define LORAWAN_KEYS_H_

#include <cstdint>

/**
 * 8 byte JoinEUI
 *
 * Pre-LoRaWAN 1.1.0, this was called AppEUI. When adding new end device in TTN,
 * you will have to enter this number. You can pick any number you want, but it
 * has to be unique.
 */
uint64_t joinEUI = 0x0000000000000000;

/**
 * 8-byte DevEUI
 *
 * This number can be anything. When adding new end device in TTN, you can
 * generate this number, or you can set any value you want, provided it is also
 * unique.
 */
uint64_t devEUI = 0x0000000000000000;

/**
 * NwkKey in MSB format
 *
 * Prior to LoRaWAN 1.1.0, this is the AppKey.
 */
uint8_t nwkKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * AppKey in MSB format
 *
 * Prior to LoRaWAN 1.1.0, only a single "nwkKey" is used. When connecting to
 * LoRaWAN 1.0 network, "appKey" will be disregarded and can be set to NULL.
 */
uint8_t appKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif // LORAWAN_KEYS_H_
