#include <stdio.h>
#include <string.h>
#include "ws2812.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t bytes[16];
    uint8_t count;
    uint8_t resets;
} fake_t;

static void fake_write(void *context, uint8_t byte)
{
    fake_t *fake = (fake_t *)context;
    if (fake->count < sizeof(fake->bytes)) fake->bytes[fake->count++] = byte;
}

static void fake_reset(void *context)
{
    ((fake_t *)context)->resets++;
}

static ws2812_port_t fake_port(fake_t *fake)
{
    ws2812_port_t port;
    port.context = fake;
    port.write_byte = fake_write;
    port.reset = fake_reset;
    return port;
}

int main(void)
{
    fake_t fake;
    ws2812_t strip;
    const uint8_t rgb[6] = {1, 2, 3, 4, 5, 6};
    const uint8_t expected[6] = {2, 1, 3, 5, 4, 6};

    memset(&fake, 0, sizeof(fake));
    ws2812_init(&strip, fake_port(&fake));
    ws2812_send_rgb(&strip, rgb, 2);
    CHECK(fake.count == 6 && memcmp(fake.bytes, expected, 6) == 0);
    CHECK(fake.resets == 1);

    memset(&fake, 0, sizeof(fake));
    ws2812_send_grb(&strip, expected, 2);
    CHECK(fake.count == 6 && memcmp(fake.bytes, expected, 6) == 0);
    CHECK(fake.resets == 1);

    if (failures) return 1;
    puts("ws2812: ok");
    return 0;
}
