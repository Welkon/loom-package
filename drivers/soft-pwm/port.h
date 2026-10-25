#ifndef SOFT_PWM_PORT_H
#define SOFT_PWM_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*set)(void *context, uint8_t level);
} soft_pwm_port_t;

#ifdef __cplusplus
}
#endif

#endif
