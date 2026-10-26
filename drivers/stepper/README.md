# stepper

Step/direction stepper driver for A4988 and DRV8825 modules.

## Application scenarios

- A linear stage or lead screw.
- A pump or valve actuator.
- A small CNC or camera slider.

## Contract

```c
void stepper_init(stepper_t *motor, stepper_port_t port,
                  uint16_t full_steps_per_rev, uint8_t microsteps);
void stepper_set_speed(stepper_t *motor, uint16_t rpm);
void stepper_move(stepper_t *motor, int32_t microsteps);
void stepper_enable(stepper_t *motor, uint8_t enabled);
```

- `stepper_move` is blocking; positive steps move forward.
- `stepper_set_speed` computes the step period from RPM, full steps, and microsteps.
- `position` counts microsteps.
- The adapter drives STEP, DIR, and optional ENABLE.

## Parameters

- A4988: 200 full steps/rev; microsteps 1, 2, 4, 8, 16.
- DRV8825: 200 full steps/rev; microsteps up to 32.
- Keep the STEP high time at least 1 us; this driver uses 2 us.
- Set the current limit before connecting the motor.

## Memory and timing

- 16 bytes; blocking move; one delay per step.
- At 600 rpm with 200 steps and 1 microstep, the period is 500 us.

## Integration

```c
stepper_t motor;
stepper_init(&motor, board_stepper_port(), 200, 16);
stepper_set_speed(&motor, 60);
stepper_enable(&motor, 1);
stepper_move(&motor, 3200);
```

## Adapters

- The adapter sets the microstep pins and the current limit.
- Reference: laurb9/StepperDriver (MIT) and the A4988/DRV8825 datasheets.
- This implementation is original.

## When not to use

- Closed-loop position control; use an encoder and a servo controller.
- High-speed motion that needs acceleration; add a ramp.

## Verification

`make test` covers direction, step count, period calculation, and enable.
