#include "stepper.h"

void stepper_init(stepper_t *motor, stepper_port_t port,
                  uint16_t full_steps_per_rev, uint8_t microsteps)
{
    if (motor == 0) {
        return;
    }
    motor->port = port;
    motor->full_steps_per_rev = full_steps_per_rev == 0U ? 200U : full_steps_per_rev;
    motor->microsteps = microsteps == 0U ? 1U : microsteps;
    motor->step_interval_us = 1000U;
    motor->position = 0;
    motor->port.set_step(motor->port.context, 0U);
    motor->port.set_direction(motor->port.context, 1U);
    motor->port.set_enable(motor->port.context, 0U);
}

void stepper_set_speed(stepper_t *motor, uint16_t rpm)
{
    uint32_t steps_per_rev;
    uint32_t rate;
    if (motor == 0 || rpm == 0U) {
        return;
    }
    steps_per_rev = (uint32_t)motor->full_steps_per_rev * motor->microsteps;
    rate = (uint32_t)rpm * steps_per_rev;
    motor->step_interval_us = 60000000UL / rate;
    if (motor->step_interval_us < 2U) {
        motor->step_interval_us = 2U;
    }
}

void stepper_move(stepper_t *motor, int32_t microsteps)
{
    uint32_t count;
    uint8_t forward;
    if (motor == 0 || microsteps == 0) {
        return;
    }
    forward = microsteps > 0 ? 1U : 0U;
    count = (uint32_t)(microsteps > 0 ? microsteps : -microsteps);
    motor->port.set_direction(motor->port.context, forward);
    while (count > 0U) {
        motor->port.set_step(motor->port.context, 1U);
        motor->port.delay_us(motor->port.context, 2U);
        motor->port.set_step(motor->port.context, 0U);
        if (motor->step_interval_us > 2U) {
            motor->port.delay_us(motor->port.context, motor->step_interval_us - 2U);
        }
        motor->position += forward ? 1 : -1;
        count--;
    }
}

void stepper_enable(stepper_t *motor, uint8_t enabled)
{
    if (motor != 0) {
        motor->port.set_enable(motor->port.context, enabled ? 1U : 0U);
    }
}
