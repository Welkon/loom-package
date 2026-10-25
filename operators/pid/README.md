# pid

Integer PID controller with integral clamping and output limits.

## Application scenarios

- Motor speed or position control.
- Heater or fan temperature control.
- A current, voltage, or flow loop on a small MCU without floating point.

## Contract

```c
void pid_init(pid_t *pid, int32_t kp, int32_t ki, int32_t kd, uint8_t shift,
              int32_t integral_min, int32_t integral_max,
              int32_t output_min, int32_t output_max);
int32_t pid_step(pid_t *pid, int32_t setpoint, int32_t measured);
void pid_reset(pid_t *pid);
```

- Gains and outputs are integers; `shift` divides the final sum by `2^shift`.
- The integral term is clamped to `[integral_min, integral_max]` (anti-windup).
- The output is clamped to `[output_min, output_max]`.
- The derivative is `error - previous_error`; call on a fixed period.
- No allocation; state is about 36 bytes.

## Parameters

- `kp`, `ki`, `kd`: tune in the same integer units as the error and output.
- `shift`: use it to keep gains in a comfortable range; 8..12 is common.
- Integral limits: set them from the expected steady-state output.
- Call period: PID assumes a fixed `dt`; the gains absorb it.

## Memory and timing

- 36 bytes; O(1), a few 64-bit multiply/divide operations.

## Integration

```c
pid_t pid;
pid_init(&pid, 80, 4, 2, 8, -2000, 2000, 0, 1000);
int32_t duty = pid_step(&pid, target_rpm, measured_rpm);
```

## When not to use

- A plant with a large dead time; add feed-forward or a different controller.
- A loop that must respect actuator rate limits; add `clamp` after the output.

## Verification

`make test` covers proportional response, integral clamping, derivative action, output limits, reset, and shift scaling.
