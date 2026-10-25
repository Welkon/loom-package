# mpu6050

MPU-6050 six-axis IMU driver.

## Application scenarios

- Tilt and orientation sensing.
- Motion detection or a shake counter.
- A logger for acceleration and angular rate.

## Contract

```c
typedef struct {
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
    int16_t temperature;
} mpu6050_raw_t;

typedef struct {
    int32_t accel_x_mg, accel_y_mg, accel_z_mg;
    int32_t gyro_x_mdps, gyro_y_mdps, gyro_z_mdps;
    int32_t temperature_milli_celsius;
} mpu6050_scaled_t;

uint8_t mpu6050_init(mpu6050_t *device, mpu6050_port_t port, uint8_t i2c_address);
uint8_t mpu6050_read_raw(mpu6050_t *device, mpu6050_raw_t *raw);
uint8_t mpu6050_read_scaled(mpu6050_t *device, mpu6050_scaled_t *scaled);
```

- `init` checks `WHO_AM_I` (0x68), resets, wakes, and sets a 1 kHz sample
  rate with a 44 Hz digital low-pass filter.
- `read_raw` reads a 14-byte burst from 0x3B.
- `read_scaled` converts to mg, milli-degrees/s, and milli-degrees Celsius.
- Returns 1 on success, 0 on a bus error or a wrong identity.

## Parameters

- Default ranges: accelerometer +-2 g, gyroscope +-250 degrees/s.
- Change the range in the adapter or extend the scale factors.
- I2C address `0x68` or `0x69`.

## Memory and timing

- About 4 bytes plus the caller's result struct.
- A burst read is 14 bytes; no delays after init.

## Integration

```c
mpu6050_t imu;
mpu6050_raw_t raw;
mpu6050_init(&imu, board_i2c_port(), 0x68);
mpu6050_read_raw(&imu, &raw);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: jrowberg/i2cdevlib MPU6050 (MIT), LibDriver `mpu6050` (MIT), and
  the InvenSense register map.
- This implementation is original.

## When not to use

- A sensor with a magnetometer for absolute heading; use a 9-axis part.
- Hard real-time attitude control; use the DMP or a dedicated filter.

## Verification

`make test` covers identity, init writes, a raw burst, scaled values, and
a wrong identity.
