#include "soft_pwm.h"

void soft_pwm_init(soft_pwm_t *pwm, soft_pwm_port_t port, uint16_t period)
{
    pwm->port = port;
    pwm->period = period == 0U ? 1U : period;
    pwm->duty = 0;
    pwm->counter = 0;
    pwm->port.set(pwm->port.context, 0);
}

void soft_pwm_set_duty(soft_pwm_t *pwm, uint16_t duty)
{
    pwm->duty = duty > pwm->period ? pwm->period : duty;
}

void soft_pwm_tick(soft_pwm_t *pwm)
{
    pwm->port.set(pwm->port.context, pwm->counter < pwm->duty ? 1U : 0U);
    pwm->counter++;
    if (pwm->counter >= pwm->period) {
        pwm->counter = 0;
    }
}
