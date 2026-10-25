#ifndef DHT_H
#define DHT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t humidity;
    int8_t temperature;
} dht11_reading_t;

typedef struct {
    uint16_t humidity_tenths;
    int16_t temperature_tenths;
} dht22_reading_t;

uint8_t dht11_decode(const uint16_t *high_us, uint8_t count, dht11_reading_t *out);
uint8_t dht22_decode(const uint16_t *high_us, uint8_t count, dht22_reading_t *out);

#ifdef __cplusplus
}
#endif

#endif
