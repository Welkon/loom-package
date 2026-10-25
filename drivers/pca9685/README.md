# pca9685

PCA9685 16-channel I2C PWM driver.

## Application scenarios

- Servo banks for a robot arm.
- LED dimming with 12-bit resolution.
- A heater or fan bank with a shared PWM frequency.

## Contract

```c
typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} pca9685_port_t;

uint8_t pca9685_init(pca9685_t *device, pca9685_port_t port,
                     uint8_t i2c_address, uint16_t frequency_hz);
uint8_t pca9685_set_pwm(pca9685_t *device, uint8_t channel,
                        uint16_t on_count, uint16_t off_count);
uint8_t pca9685_set_all(pca9685_t *device, uint16_t on_count, uint16_t off_count);
uint8_t pca9685_set_frequency(pca9685_t *device, uint16_t frequency_hz);
```

- Channels are 0..15; counts are 0..4095.
- `set_pwm` writes the four LED registers for one channel.
- `set_frequency` sleeps the oscillator, writes the prescaler, wakes, and restarts.
- Returns 1 on success, 0 on a bad argument or a bus error.

## Parameters

- 24-25 MHz internal oscillator; the prescaler is computed from it.
- Servo frames usually use 50 Hz; LEDs can use 200 Hz..1 kHz.
- The I2C address is `0x40`..`0x7F`.

## Memory and timing

- 4 bytes; one 4-byte write per channel update.
- A frequency change takes about 1 ms.

## Integration

```c
pca9685_t pwm;
pca9685_init(&pwm, board_i2c_port(), 0x40, 50);
pca9685_set_pwm(&pwm, 0, 0, 300);   /* 1.5 ms pulse */
```

## Adapters

- The adapter handles the register pointer and repeated start.
- Reference: LibDriver `pca9685` (MIT), Adafruit PCA9685, and the NXP datasheet.
- This implementation is original.

## When not to use

- A single PWM channel; use the MCU timer.
- A servo that needs closed-loop position feedback.

## Verification

`make test` covers the init prescaler, a channel write, all-channel write,
and a frequency change.
