#include <stdio.h>
#include "soft_pwm.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t levels[16];
    uint8_t count;
} fake_t;

static void fake_set(void *context, uint8_t level)
{
    fake_t *fake = (fake_t *)context;
    if (fake->count < sizeof(fake->levels)) {
        fake->levels[fake->count++] = level;
    }
}

static soft_pwm_port_t fake_port(fake_t *fake)
{
    soft_pwm_port_t port;
    port.context = fake;
    port.set = fake_set;
    return port;
}

int main(void)
{
    fake_t fake = {{0}, 0};
    soft_pwm_t pwm;

    soft_pwm_init(&pwm, fake_port(&fake), 4);
    soft_pwm_set_duty(&pwm, 2);
    CHECK(fake.count == 1 && fake.levels[0] == 0);
    for (int i = 0; i < 4; i++) soft_pwm_tick(&pwm);
    CHECK(fake.count == 5);
    CHECK(fake.levels[1] == 1 && fake.levels[2] == 1);
    CHECK(fake.levels[3] == 0 && fake.levels[4] == 0);

    soft_pwm_set_duty(&pwm, 10);
    CHECK(pwm.duty == 4);
    soft_pwm_set_duty(&pwm, 0);
    CHECK(pwm.duty == 0);

    soft_pwm_init(&pwm, fake_port(&fake), 0);
    CHECK(pwm.period == 1);

    if (failures) return 1;
    puts("soft_pwm: ok");
    return 0;
}
