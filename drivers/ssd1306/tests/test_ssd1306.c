#include <stdio.h>
#include <string.h>
#include "ssd1306.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t controls[2048];
    uint8_t bytes[2048];
    uint16_t count;
} fake_t;

static uint8_t fake_write(void *context, uint8_t control, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    if (fake->count < sizeof(fake->controls)) fake->controls[fake->count] = control;
    for (uint16_t i = 0; i < length && fake->count < sizeof(fake->bytes); i++) {
        fake->bytes[fake->count++] = data[i];
    }
    return 1;
}

static ssd1306_port_t fake_port(fake_t *fake)
{
    ssd1306_port_t port;
    port.context = fake;
    port.write = fake_write;
    return port;
}

int main(void)
{
    fake_t fake;
    uint8_t page[4] = {1, 2, 3, 4};

    memset(&fake, 0, sizeof(fake));
    ssd1306_init(fake_port(&fake));
    CHECK(fake.count == 26);
    CHECK(fake.controls[0] == 0x00);
    CHECK(fake.bytes[0] == 0xAE && fake.bytes[1] == 0xD5);
    CHECK(fake.bytes[fake.count - 1] == 0xAF);

    memset(&fake, 0, sizeof(fake));
    ssd1306_set_contrast(fake_port(&fake), 0x7F);
    CHECK(fake.count == 2 && fake.bytes[0] == 0x81 && fake.bytes[1] == 0x7F);

    memset(&fake, 0, sizeof(fake));
    ssd1306_clear(fake_port(&fake));
    CHECK(fake.count == 8U * (3U + 8U * 16U));
    CHECK(fake.controls[0] == 0x00 && fake.controls[3] == 0x40);

    memset(&fake, 0, sizeof(fake));
    ssd1306_write_page(fake_port(&fake), 2, 4, page, 4);
    CHECK(fake.count == 3 + 4);
    CHECK(fake.bytes[0] == 0xB2 && fake.bytes[1] == 0x04 && fake.bytes[2] == 0x10);
    CHECK(fake.controls[3] == 0x40);

    if (failures) return 1;
    puts("ssd1306: ok");
    return 0;
}
