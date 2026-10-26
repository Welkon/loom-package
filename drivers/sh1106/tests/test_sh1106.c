#include <stdio.h>
#include <string.h>
#include "sh1106.h"

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

int main(void)
{
    fake_t fake;
    sh1106_port_t port;
    uint8_t page[4] = {1, 2, 3, 4};

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    sh1106_init(port);
    CHECK(fake.count == 22);
    CHECK(fake.controls[0] == 0x00 && fake.bytes[0] == 0xAE);
    CHECK(fake.bytes[1] == 0xA4 && fake.bytes[9] == 0xAD);
    CHECK(fake.bytes[fake.count - 1] == 0xAF);

    memset(&fake, 0, sizeof(fake));
    sh1106_set_contrast(port, 0x7F);
    CHECK(fake.count == 2 && fake.bytes[0] == 0x81 && fake.bytes[1] == 0x7F);

    memset(&fake, 0, sizeof(fake));
    sh1106_clear(port);
    CHECK(fake.count == 8U * (3U + 8U * 16U));
    CHECK(fake.controls[0] == 0x00 && fake.controls[3] == 0x40);
    CHECK(fake.bytes[1] == 0x02 && fake.bytes[2] == 0x10);

    memset(&fake, 0, sizeof(fake));
    sh1106_write_page(port, 2, 4, page, 4);
    CHECK(fake.count == 3 + 4);
    CHECK(fake.bytes[0] == 0xB2);
    CHECK(fake.bytes[1] == 0x06 && fake.bytes[2] == 0x10);
    CHECK(fake.controls[3] == 0x40);

    if (failures) return 1;
    puts("sh1106: ok");
    return 0;
}
