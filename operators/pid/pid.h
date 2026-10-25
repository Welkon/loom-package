#ifndef PID_H
#define PID_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t kp;
    int32_t ki;
    int32_t kd;
    int32_t integral;
    int32_t integral_min;
    int32_t integral_max;
    int32_t output_min;
    int32_t output_max;
    int32_t previous_error;
    uint8_t shift;
} pid_t;

void pid_init(pid_t *pid, int32_t kp, int32_t ki, int32_t kd, uint8_t shift,
              int32_t integral_min, int32_t integral_max,
              int32_t output_min, int32_t output_max);
int32_t pid_step(pid_t *pid, int32_t setpoint, int32_t measured);
void pid_reset(pid_t *pid);

#ifdef __cplusplus
}
#endif

#endif
