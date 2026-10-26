#include "hx711.h"

static uint8_t wait_ready(hx711_t *device)
{
    uint32_t waited = 0;
    while (device->port.read_dout(device->port.context) != 0U) {
        if (waited >= 100000UL) {
            return 0;
        }
        if (device->port.delay_us != 0) {
            device->port.delay_us(device->port.context, 10U);
        }
        waited += 10U;
    }
    return 1;
}

uint8_t hx711_init(hx711_t *device, hx711_port_t port)
{
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->gain = 128U;
    device->port.set_sck(device->port.context, 0U);
    return 1;
}

uint8_t hx711_ready(hx711_t *device)
{
    return device != 0 && device->port.read_dout(device->port.context) == 0U;
}

uint8_t hx711_read(hx711_t *device, int32_t *value)
{
    uint32_t raw = 0;
    uint8_t i;
    uint8_t pulses;
    if (device == 0 || value == 0 || !wait_ready(device)) {
        return 0;
    }
    for (i = 0; i < 24U; i++) {
        device->port.set_sck(device->port.context, 1U);
        if (device->port.delay_us != 0) {
            device->port.delay_us(device->port.context, 1U);
        }
        raw = (raw << 1) | (device->port.read_dout(device->port.context) & 1U);
        device->port.set_sck(device->port.context, 0U);
        if (device->port.delay_us != 0) {
            device->port.delay_us(device->port.context, 1U);
        }
    }
    pulses = device->gain == 64U ? 3U : (device->gain == 32U ? 2U : 1U);
    for (i = 0; i < pulses; i++) {
        device->port.set_sck(device->port.context, 1U);
        if (device->port.delay_us != 0) {
            device->port.delay_us(device->port.context, 1U);
        }
        device->port.set_sck(device->port.context, 0U);
        if (device->port.delay_us != 0) {
            device->port.delay_us(device->port.context, 1U);
        }
    }
    if ((raw & 0x800000UL) != 0U) {
        raw |= 0xFF000000UL;
    }
    *value = (int32_t)raw;
    return 1;
}
