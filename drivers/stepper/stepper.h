#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*set_step)(void *context, uint8_t level);
    void (*set_direction)(void *context, uint8_t forward);
    void (*set_enable)(void *context, uint8_t enabled);
    void (*delay_us)(void *context, uint32_t microseconds);
} stepper_port_t;

typedef struct {
    stepper_port_t port;
    uint16_t full_steps_per_rev;
    uint8_t microsteps;
    uint32_t step_interval_us;
    int32_t position;
} stepper_t;

void stepper_init(stepper_t *motor, stepper_port_t port,
                  uint16_t full_steps_per_rev, uint8_t microsteps);
void stepper_set_speed(stepper_t *motor, uint16_t rpm);
void stepper_move(stepper_t *motor, int32_t microsteps);
void stepper_enable(stepper_t *motor, uint8_t enabled);

#ifdef __cplusplus
}
#endif

#endif
