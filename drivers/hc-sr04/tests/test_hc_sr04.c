#include <stdio.h>
#include "hc_sr04.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint32_t echo;
    uint16_t triggers;
} fake_t;

static void fake_trigger(void *context)
{
    fake_t *fake = (fake_t *)context;
    fake->triggers++;
}
static uint32_t fake_echo(void *context)
{
    fake_t *fake = (fake_t *)context;
    return fake->echo;
}

int main(void)
{
    fake_t fake = {5800, 0};
    hc_sr04_port_t port = {&fake, fake_trigger, fake_echo};
    uint32_t mm;

    CHECK(hc_sr04_millimeters(5800) == 994);
    CHECK(hc_sr04_millimeters(100) == 17);
    CHECK(hc_sr04_read(port, &mm) == 1 && mm == 994 && fake.triggers == 1);

    fake.echo = 60000;
    CHECK(hc_sr04_read(port, &mm) == 1 && mm == 10290);
    fake.echo = 99;
    CHECK(hc_sr04_read(port, &mm) == 0);
    fake.echo = 60001;
    CHECK(hc_sr04_read(port, &mm) == 0);

    if (failures) return 1;
    puts("hc_sr04: ok");
    return 0;
}
