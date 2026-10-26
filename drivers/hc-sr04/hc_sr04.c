#include "hc_sr04.h"

uint32_t hc_sr04_millimeters(uint32_t echo_us)
{
    return (uint32_t)(((uint64_t)echo_us * 343U) / 2000U);
}

uint8_t hc_sr04_read(hc_sr04_port_t port, uint32_t *millimeters)
{
    uint32_t echo_us;
    if (millimeters == 0) {
        return 0;
    }
    port.trigger(port.context);
    echo_us = port.echo_pulse_us(port.context);
    if (echo_us < 100U || echo_us > 60000U) {
        return 0;
    }
    *millimeters = hc_sr04_millimeters(echo_us);
    return 1;
}
