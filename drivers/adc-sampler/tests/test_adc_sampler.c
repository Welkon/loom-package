#include <stdio.h>
#include "adc_sampler.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint16_t values[8];
    uint8_t count;
    uint8_t index;
} fake_t;

static uint16_t fake_read(void *context, uint8_t channel)
{
    fake_t *fake = (fake_t *)context;
    (void)channel;
    if (fake->index >= fake->count) return 0;
    return fake->values[fake->index++];
}

static adc_port_t fake_port(fake_t *fake)
{
    adc_port_t port;
    port.context = fake;
    port.read = fake_read;
    return port;
}

int main(void)
{
    fake_t fake = {{10, 20, 30, 40}, 4, 0};
    adc_sampler_t adc;

    adc_sampler_init(&adc, fake_port(&fake), 4);
    CHECK(adc_sampler_read(&adc, 0) == 25);
    CHECK(adc_sampler_read(&adc, 0) == 0);

    fake.index = 0;
    adc_sampler_init(&adc, fake_port(&fake), 0);
    CHECK(adc_sampler_read(&adc, 0) == 10);

    CHECK(adc_sampler_mv(&adc, 2048, 12, 3300) == 1650);
    CHECK(adc_sampler_mv(&adc, 4095, 12, 3300) == 3299);
    CHECK(adc_sampler_mv(&adc, 100, 0, 3300) == 0);

    if (failures) return 1;
    puts("adc_sampler: ok");
    return 0;
}
