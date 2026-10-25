#include "ds18b20.h"

static void write_byte(onewire_port_t port, uint8_t value)
{
    uint8_t bit;
    for (bit = 0; bit < 8U; bit++) {
        port.write_bit(port.context, (uint8_t)((value >> bit) & 1U));
    }
}

static uint8_t read_byte(onewire_port_t port)
{
    uint8_t bit;
    uint8_t value = 0;
    for (bit = 0; bit < 8U; bit++) {
        value |= (uint8_t)(port.read_bit(port.context) << bit);
    }
    return value;
}

static uint8_t crc8(const uint8_t *data, uint8_t length)
{
    uint8_t crc = 0;
    uint8_t index;
    for (index = 0; index < length; index++) {
        uint8_t bit;
        crc ^= data[index];
        for (bit = 0; bit < 8U; bit++) {
            crc = (crc & 0x01U) ? (uint8_t)((crc >> 1) ^ 0x8CU) : (uint8_t)(crc >> 1);
        }
    }
    return crc;
}

uint8_t ds18b20_read(onewire_port_t port, int16_t *temperature_q4)
{
    uint8_t scratchpad[9];
    uint8_t index;

    if (temperature_q4 == 0 || !port.reset(port.context)) {
        return 0;
    }
    write_byte(port, 0xCCU);
    write_byte(port, 0x44U);
    port.delay_ms(port.context, 750);

    if (!port.reset(port.context)) {
        return 0;
    }
    write_byte(port, 0xCCU);
    write_byte(port, 0xBEU);
    for (index = 0; index < 9U; index++) {
        scratchpad[index] = read_byte(port);
    }
    if (crc8(scratchpad, 8U) != scratchpad[8]) {
        return 0;
    }
    *temperature_q4 = (int16_t)(((uint16_t)scratchpad[1] << 8) | scratchpad[0]);
    return 1;
}
