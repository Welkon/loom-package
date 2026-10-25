#include "pid.h"

void pid_init(pid_t *pid, int32_t kp, int32_t ki, int32_t kd, uint8_t shift,
              int32_t integral_min, int32_t integral_max,
              int32_t output_min, int32_t output_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0;
    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->previous_error = 0;
    pid->shift = shift > 30U ? 30U : shift;
}

int32_t pid_step(pid_t *pid, int32_t setpoint, int32_t measured)
{
    int32_t error = setpoint - measured;
    int64_t integral = (int64_t)pid->integral + error;
    int32_t derivative;
    int64_t sum;
    int64_t divisor;
    int32_t output;

    if (integral < pid->integral_min) {
        integral = pid->integral_min;
    } else if (integral > pid->integral_max) {
        integral = pid->integral_max;
    }
    pid->integral = (int32_t)integral;
    derivative = error - pid->previous_error;
    sum = (int64_t)pid->kp * error
        + (int64_t)pid->ki * integral
        + (int64_t)pid->kd * derivative;
    divisor = (int64_t)1 << pid->shift;
    output = (int32_t)(sum / divisor);
    if (output < pid->output_min) {
        output = pid->output_min;
    } else if (output > pid->output_max) {
        output = pid->output_max;
    }
    pid->previous_error = error;
    return output;
}

void pid_reset(pid_t *pid)
{
    pid->integral = 0;
    pid->previous_error = 0;
}
