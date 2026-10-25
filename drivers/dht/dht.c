#include "dht.h"

static uint8_t bits_to_bytes(const uint16_t *high_us, uint8_t count, uint8_t *bytes)
{
    uint8_t i;
    uint8_t index;
    if (high_us == 0 || bytes == 0 || count != 40U) {
        return 0;
    }
    for (i = 0; i < 5U; i++) {
        bytes[i] = 0;
    }
    for (i = 0; i < 40U; i++) {
        if (high_us[i] >= 50U) {
            bytes[i / 8U] |= (uint8_t)(1U << (7U - (i % 8U)));
        }
    }
    index = (uint8_t)(bytes[0] + bytes[1] + bytes[2] + bytes[3]);
    return index == bytes[4] ? 1U : 0U;
}

uint8_t dht11_decode(const uint16_t *high_us, uint8_t count, dht11_reading_t *out)
{
    uint8_t bytes[5];
    if (out == 0 || !bits_to_bytes(high_us, count, bytes)) {
        return 0;
    }
    out->humidity = bytes[0];
    out->temperature = (int8_t)bytes[2];
    return 1;
}

uint8_t dht22_decode(const uint16_t *high_us, uint8_t count, dht22_reading_t *out)
{
    uint8_t bytes[5];
    uint16_t raw_temperature;
    if (out == 0 || !bits_to_bytes(high_us, count, bytes)) {
        return 0;
    }
    out->humidity_tenths = (uint16_t)(((uint16_t)bytes[0] << 8) | bytes[1]);
    raw_temperature = (uint16_t)(((uint16_t)bytes[2] << 8) | bytes[3]);
    if ((raw_temperature & 0x8000U) != 0U) {
        out->temperature_tenths = (int16_t)-(int16_t)(raw_temperature & 0x7FFFU);
    } else {
        out->temperature_tenths = (int16_t)raw_temperature;
    }
    return 1;
}
