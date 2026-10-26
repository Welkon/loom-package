# scd4x

Sensirion SCD40/SCD41 CO2, temperature, and humidity sensor.

## Application scenarios

- Indoor air quality.
- A ventilation controller.
- A greenhouse or classroom CO2 monitor.

## Contract

```c
uint8_t scd4x_init(scd4x_t *sensor, scd4x_port_t port, uint8_t i2c_address);
uint8_t scd4x_read(scd4x_t *sensor, uint16_t *co2_ppm,
                   int32_t *milli_celsius, uint32_t *milli_percent);
```

- `init` stops any running measurement, then starts periodic measurement.
- `read` polls data-ready, reads nine bytes, and validates all three CRCs.
- CO2 is ppm; temperature is millidegrees Celsius; humidity is
  millipercent RH.
- Returns 1 on success, 0 on a bus error, a bad CRC, or a timeout.

## Parameters

- I2C address `0x62`.
- CO2 range 400..5000 ppm; temperature -10..60 C; humidity 0..100 %RH.
- The first measurement needs about 5 s; later ones arrive every 5 s.

## Memory and timing

- 8 bytes; one 9-byte read.
- A data-ready poll waits up to 5 s in 100 ms steps.

## Integration

```c
scd4x_t sensor;
uint16_t co2;
int32_t temperature;
uint32_t humidity;
scd4x_init(&sensor, board_i2c_port(), 0x62);
if (scd4x_read(&sensor, &co2, &temperature, &humidity)) {
    show(co2, temperature / 1000, humidity / 1000);
}
```

## Adapters

- The adapter writes a two-byte command and reads the response.
- Reference: LibDriver `scd4x` (MIT) and the Sensirion SCD4x datasheet.
- This implementation is original.

## When not to use

- A fast gas measurement; the CO2 measurement period is seconds.
- A condensing environment without a protective membrane.

## Verification

`make test` covers the command sequence, all three CRCs, conversion, and a
bad CRC.
