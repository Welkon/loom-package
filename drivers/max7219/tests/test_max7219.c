#include <stdio.h>
#include <string.h>
#include "max7219.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint16_t frames[64];
    uint8_t count;
    uint8_t high;
    uint8_t phase;
} fake_t;

static void fake_select(void *context)
{
    fake_t *fake = (fake_t *)context;
    fake->phase = 0;
}

static uint8_t fake_transfer(void *context, uint8_t value)
{
    fake_t *fake = (fake_t *)context;
    if (fake->phase == 0) {
        fake->high = value;
        fake->phase = 1;
    } else {
        if (fake->count < sizeof(fake->frames) / sizeof(fake->frames[0])) {
            fake->frames[fake->count++] = (uint16_t)(((uint16_t)fake->high << 8) | value);
        }
        fake->phase = 0;
    }
    return 0;
}

static void fake_deselect(void *context)
{
    (void)context;
}

int main(void)
{
    fake_t fake;
    max7219_t display;

    memset(&fake, 0, sizeof(fake));
    max7219_init(&display, (max7219_port_t){&fake, fake_select, fake_transfer, fake_deselect}, 2);
    CHECK(display.devices == 2);
    CHECK(fake.count > 0);

    fake.count = 0;
    max7219_write(&display, 1, 0x0A, 0x05);
    CHECK(fake.count == 2);
    CHECK(fake.frames[0] == 0x0A05);
    CHECK(fake.frames[1] == 0x0000);

    fake.count = 0;
    max7219_write(&display, 0, 0x0A, 0x05);
    CHECK(fake.count == 2);
    CHECK(fake.frames[0] == 0x0000);
    CHECK(fake.frames[1] == 0x0A05);

    fake.count = 0;
    max7219_set_digit(&display, 0, 2, 0x0F);
    CHECK(fake.count == 2 && fake.frames[1] == 0x030F);
    max7219_set_intensity(&display, 0, 0x1F);
    max7219_shutdown(&display, 0, 0);
    CHECK(fake.frames[fake.count - 1] == 0x0C01);

    if (failures) return 1;
    puts("max7219: ok");
    return 0;
}
