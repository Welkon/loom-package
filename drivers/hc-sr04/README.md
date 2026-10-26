# hc-sr04

HC-SR04 ultrasonic distance sensor.

## Application scenarios

- A parking or obstacle sensor.
- Tank or bin level measurement.
- A simple presence detector.

## Contract

```c
uint8_t hc_sr04_read(hc_sr04_port_t port, uint32_t *millimeters);
uint32_t hc_sr04_millimeters(uint32_t echo_us);
```

- `read` triggers one measurement and measures the echo pulse.
- `millimeters` is the round-trip distance at 343 m/s.
- Returns 1 on success, 0 for an out-of-range echo.
- `hc_sr04_millimeters` is the pure conversion, useful for capture ISRs.

## Parameters

- Measure echoes from about 100 us to 60 ms.
- Keep the sensor perpendicular to a flat target.
- The speed of sound changes with temperature; this unit uses 343 m/s.

## Memory and timing

- 4 bytes; one trigger and one pulse measurement.
- A 60 ms maximum echo limits the range to about 10 m.

## Integration

```c
uint32_t millimeters;
if (hc_sr04_read(board_hc_sr04_port(), &millimeters)) {
    show(millimeters);
}
```

## Adapters

- The adapter drives TRIG high for 10 us, then measures ECHO high time.
- Reference: LibDriver `hcsr04` (MIT) and the HC-SR04 datasheet.
- This implementation is original.

## When not to use

- A soft or angled target; use a different ranging principle.
- Multiple sensors without cross-talk sequencing.

## Verification

`make test` covers a nominal echo, the minimum and maximum range, and a
short/invalid echo.
