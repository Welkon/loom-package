# soft-pwm

Software PWM on a single pin, driven by a fixed tick.

## Application scenarios

- An LED or heater on a low-end MCU with no spare hardware PWM.
- A fan or solenoid that needs a slow duty cycle.
- A status LED that must share a timer with other work.

## Contract

```c
typedef struct {
    void *context;
    void (*set)(void *context, uint8_t level);
} soft_pwm_port_t;

void soft_pwm_init(soft_pwm_t *pwm, soft_pwm_port_t port, uint16_t period);
void soft_pwm_set_duty(soft_pwm_t *pwm, uint16_t duty);
void soft_pwm_tick(soft_pwm_t *pwm);
```

- Call `soft_pwm_tick` at a fixed rate; `period` is the number of ticks.
- Duty is clamped to `0..period`.
- `period = 0` becomes 1.
- No allocation.

## Parameters

- Tick rate / period sets the PWM frequency; keep it above visible flicker
  for LEDs (>= 100 Hz) and below the actuator's response limit.
- Duty resolution is `1/period`.

## Memory and timing

- 12 bytes of state plus the port context.
- O(1) per tick; one pin write.

## Integration

```c
soft_pwm_t led;
soft_pwm_init(&led, board_led_port(), 100);
soft_pwm_set_duty(&led, 25);
/* every 100 us: */ soft_pwm_tick(&led);
```

## Adapters

- Keep the pin write and timer setup in the adapter.
- Reference: Padauk PWM/LED examples; CA51 low-end projects.
- Prefer hardware PWM when a channel is free.

## When not to use

- High frequency or low jitter is required; use a hardware timer.
- The tick is not periodic; the duty will drift.

## Verification

`make test` covers the 0/50/100% duty sequence, clamping, and period zero.
