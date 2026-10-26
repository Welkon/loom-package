#include <stdio.h>
#include <string.h>
#include "hd44780.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

typedef struct {
    uint8_t rs;
    uint8_t nibbles[128];
    uint16_t count;
    uint16_t pulses;
    uint16_t delays;
} fake_t;

static void fake_rs(void *context, uint8_t level) { ((fake_t *)context)->rs = level; }
static void fake_enable(void *context, uint8_t level) { if (level) ((fake_t *)context)->pulses++; }
static void fake_nibble(void *context, uint8_t value)
{
    fake_t *fake = (fake_t *)context;
    if (fake->count < sizeof(fake->nibbles)) fake->nibbles[fake->count++] = value;
}
static void fake_delay(void *context, uint16_t ms)
{
    fake_t *fake = (fake_t *)context;
    (void)ms;
    fake->delays++;
}

int main(void)
{
    fake_t fake;
    hd44780_t lcd;
    hd44780_port_t port;
    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.set_rs = fake_rs;
    port.set_enable = fake_enable;
    port.write_nibble = fake_nibble;
    port.delay_ms = fake_delay;
    CHECK(hd44780_init(&lcd, port, 16, 2) == 1);
    CHECK(fake.count > 10 && fake.pulses == fake.count);
    CHECK(fake.nibbles[0] == 0x03 && fake.nibbles[1] == 0x03 && fake.nibbles[2] == 0x03);
    CHECK(fake.nibbles[3] == 0x02 && fake.nibbles[4] == 0x02 && fake.nibbles[5] == 0x08);

    fake.count = 0;
    fake.rs = 0;
    hd44780_write_char(&lcd, 'A');
    CHECK(fake.count == 2 && fake.nibbles[0] == 0x04 && fake.nibbles[1] == 0x01);
    CHECK(fake.rs == 1);

    fake.count = 0;
    hd44780_set_cursor(&lcd, 3, 1);
    CHECK(fake.count == 2 && fake.nibbles[0] == 0x0C && fake.nibbles[1] == 0x03);

    CHECK(hd44780_init(&lcd, port, 21, 2) == 0);
    CHECK(hd44780_init(&lcd, port, 16, 3) == 0);

    if (failures) return 1;
    puts("hd44780: ok");
    return 0;
}
