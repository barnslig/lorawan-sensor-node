/* lorawan-keys.h defines LoRaWAN keys needed by the LMIC library.
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

// Keys required for OTAA activation:

// End-device Identifier (u1_t[8]) in lsb format
#define OTAA_DEVEUI 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

// Application Identifier (u1_t[8]) in lsb format
#define OTAA_APPEUI 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

// Application Key (u1_t[16]) in msb format
#define OTAA_APPKEY 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#endif // LORAWAN_KEYS_H_
