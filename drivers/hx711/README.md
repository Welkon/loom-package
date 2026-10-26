# hx711

HX711 24-bit load-cell amplifier.

## Application scenarios

- A kitchen or platform scale.
- A force or torque sensor.
- A tension controller.

## Contract

```c
uint8_t hx711_init(hx711_t *device, hx711_port_t port);
uint8_t hx711_ready(hx711_t *device);
uint8_t hx711_read(hx711_t *device, int32_t *value);
```

- `read` waits for DOUT low, clocks 24 bits, then sends the gain pulses.
- Gain 128 selects channel A; gain 64 selects channel A at 64; gain 32
  selects channel B.
- Returns 1 on success, 0 on a timeout.
- The result is sign-extended 24-bit raw data.

## Parameters

- Set `gain` to 128, 64, or 32 before a read; it applies to the next sample.
- The clock high time must stay below 60 us.
- A load-cell excitation of 5 V and a known mV/V rating are needed to scale.

## Memory and timing

- About 8 bytes; one read takes 24 clock pulses plus 1..3 gain pulses.
- The conversion rate is 10 or 80 samples/s depending on the module.

## Integration

```c
hx711_t scale;
hx711_init(&scale, board_hx711_port());
scale.gain = 128;
int32_t raw;
if (hx711_read(&scale, &raw)) {
    weight = (raw - offset) / scale_factor;
}
```

## Adapters

- The adapter reads DOUT and drives SCK; do not stretch a clock pulse.
- Reference: bogde/HX711 (MIT) and the Avia HX711 datasheet.
- This implementation is original.

## When not to use

- A load cell with an SPI ADC; use that converter.
- A high-vibration environment without mechanical filtering.

## Verification

`make test` covers a positive value, a negative value, gain pulses, and a
timeout.
