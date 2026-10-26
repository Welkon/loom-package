#include "max31855.h"

uint8_t max31855_read(max31855_port_t port, max31855_reading_t *reading)
{
    uint32_t frame = 0;
    uint8_t i;
    int16_t raw;
    if (reading == 0) {
        return 0;
    }
    port.select(port.context);
    for (i = 0; i < 4U; i++) {
        frame = (frame << 8) | port.transfer(port.context, 0x00U);
    }
    port.deselect(port.context);

    reading->fault = (uint8_t)((frame >> 16) & 0x01U);
    reading->open_circuit = (uint8_t)(frame & 0x01U);
    reading->short_to_gnd = (uint8_t)((frame >> 1) & 0x01U);
    reading->short_to_vcc = (uint8_t)((frame >> 2) & 0x01U);

    raw = (int16_t)(frame >> 16);
    raw = (int16_t)(raw >> 2);
    reading->thermocouple_milli_c = (int32_t)raw * 250;

    raw = (int16_t)((frame >> 4) & 0x0FFFU);
    if ((raw & 0x0800) != 0) {
        raw = (int16_t)(raw | 0xF000U);
    }
    reading->internal_milli_c = (int32_t)raw * 125 / 2;
    return 1;
}
