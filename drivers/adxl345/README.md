# adxl345

ADXL345 three-axis accelerometer.

## Application scenarios

- Tilt and orientation sensing.
- Vibration or shock logging.
- A motion-triggered wake-up.

## Contract

```c
uint8_t adxl345_init(adxl345_t *device, adxl345_port_t port, uint8_t i2c_address);
uint8_t adxl345_read(adxl345_t *device, adxl345_reading_t *reading);
```

- `init` checks `DEVID` (0xE5), sets 100 Hz, full resolution, +-16 g, and
  measurement mode.
- `read` reads six bytes from 0x32 and converts to mg (3.9 mg/LSB).
- Returns 1 on success, 0 on a bus error or a wrong identity.

## Parameters

- I2C address `0x53` (ALT high) or `0x1D` (ALT low).
- Full resolution is 3.9 mg/LSB over +-16 g.
- The data format can be changed in the adapter.

## Memory and timing

- About 4 bytes plus the result.
- One 6-byte read; no delay after init.

## Integration

```c
adxl345_t accel;
adxl345_reading_t sample;
adxl345_init(&accel, board_i2c_port(), 0x53);
adxl345_read(&accel, &sample);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: LibDriver `adxl345` (MIT) and the Analog Devices datasheet.
- This implementation is original.

## When not to use

- A gyroscope is also needed; use an IMU.
- High-g shock beyond +-16 g; choose a wider-range part.

## Verification

`make test` covers identity, init registers, a signed read, and scaling.
