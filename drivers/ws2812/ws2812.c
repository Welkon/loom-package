#include "ws2812.h"

void ws2812_init(ws2812_t *strip, ws2812_port_t port)
{
    strip->port = port;
}

void ws2812_send_rgb(ws2812_t *strip, const uint8_t *rgb, uint16_t count)
{
    uint16_t index;
    if (rgb == 0) {
        return;
    }
    for (index = 0; index < count; index++) {
        const uint8_t *pixel = &rgb[index * 3U];
        strip->port.write_byte(strip->port.context, pixel[1]);
        strip->port.write_byte(strip->port.context, pixel[0]);
        strip->port.write_byte(strip->port.context, pixel[2]);
    }
    strip->port.reset(strip->port.context);
}

void ws2812_send_grb(ws2812_t *strip, const uint8_t *grb, uint16_t count)
{
    uint16_t index;
    if (grb == 0) {
        return;
    }
    for (index = 0; index < count * 3U; index++) {
        strip->port.write_byte(strip->port.context, grb[index]);
    }
    strip->port.reset(strip->port.context);
}
