# bme280

BME280 temperature, pressure, and humidity sensor.

## Application scenarios

- Weather stations and room climate.
- Altitude estimation from pressure.
- Compensated humidity in a greenhouse or enclosure.

## Contract

```c
uint8_t bme280_init(bme280_t *device, bme280_port_t port, uint8_t i2c_address);
uint8_t bme280_read(bme280_t *device, bme280_reading_t *reading);
```

- `read` triggers a forced measurement, waits 10 ms, and compensates all three
  channels with the factory calibration.
- Temperature is millidegrees Celsius; pressure is pascals; humidity is
  millipercent RH.
- Returns 1 on success, 0 on a bus error or a wrong identity.

## Parameters

- I2C address `0x76` or `0x77`.
- Oversampling is x1 for temperature, pressure, and humidity.
- Pressure is clamped to 30000..110000 Pa; humidity to 0..100 %RH.

## Memory and timing

- About 60 bytes for calibration; one 8-byte read.
- A forced measurement takes about 10 ms.

## Integration

```c
bme280_t weather;
bme280_reading_t sample;
bme280_init(&weather, board_i2c_port(), 0x76);
bme280_read(&weather, &sample);
show(sample.temperature_milli_c / 1000, sample.pressure_pa, sample.humidity_milli_percent);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: BoschSensortec/BME280_driver (BSD-3-Clause), LibDriver `bme280`
  (MIT), and the BME280 datasheet.
- This implementation is original and uses the integer compensation formulas.

## When not to use

- A part that is already calibrated in a higher-level driver.
- A condensing environment without a protective membrane.

## Verification

`make test` covers calibration parsing, a forced read, signed temperature,
pressure, humidity, and a wrong identity.
