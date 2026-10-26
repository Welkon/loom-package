#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stepper.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint16_t high;
    uint16_t low;
    uint16_t directions;
    uint16_t enables;
    uint32_t delay_total;
} fake_t;

static void fake_step(void *context, uint8_t level)
{
    fake_t *fake = (fake_t *)context;
    if (level) fake->high++; else fake->low++;
}
static void fake_direction(void *context, uint8_t forward)
{
    fake_t *fake = (fake_t *)context;
    (void)forward;
    fake->directions++;
}
static void fake_enable(void *context, uint8_t enabled)
{
    fake_t *fake = (fake_t *)context;
    (void)enabled;
    fake->enables++;
}
static void fake_delay(void *context, uint32_t us)
{
    fake_t *fake = (fake_t *)context;
    fake->delay_total += us;
}

int main(void)
{
    fake_t fake;
    stepper_t motor;

    memset(&fake, 0, sizeof(fake));
    stepper_init(&motor, (stepper_port_t){&fake, fake_step, fake_direction, fake_enable, fake_delay}, 200, 1);
    CHECK(motor.position == 0 && fake.high == 0 && fake.low == 1);
    stepper_set_speed(&motor, 600);
    CHECK(motor.step_interval_us == 500);

    stepper_move(&motor, 4);
    CHECK(fake.high == 4 && fake.low == 5);
    CHECK(fake.directions == 2);
    CHECK(motor.position == 4);
    CHECK(fake.delay_total == 4U * 500U);

    stepper_move(&motor, -2);
    CHECK(motor.position == 2);
    CHECK(fake.high == 6);
    stepper_enable(&motor, 1);
    CHECK(fake.enables == 2);

    stepper_set_speed(&motor, 0);
    CHECK(motor.step_interval_us == 500);
    stepper_move(&motor, 0);
    CHECK(motor.position == 2);

    if (failures) return 1;
    puts("stepper: ok");
    return 0;
}
