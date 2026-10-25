#ifndef SOFT_PWM_H
#define SOFT_PWM_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    soft_pwm_port_t port;
    uint16_t period;
    uint16_t duty;
    uint16_t counter;
} soft_pwm_t;

void soft_pwm_init(soft_pwm_t *pwm, soft_pwm_port_t port, uint16_t period);
void soft_pwm_set_duty(soft_pwm_t *pwm, uint16_t duty);
void soft_pwm_tick(soft_pwm_t *pwm);

#ifdef __cplusplus
}
#endif

#endif
