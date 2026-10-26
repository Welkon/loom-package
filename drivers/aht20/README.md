# aht20

AHT20 temperature and humidity sensor.

## Application scenarios

- Low-cost room climate sensing.
- Enclosure temperature and humidity.
- A calibrated sensor with a digital I2C interface.

## Contract

```c
uint8_t aht20_init(aht20_t *device, aht20_port_t port, uint8_t i2c_address);
uint8_t aht20_read(aht20_t *device, int32_t *milli_celsius, int32_t *milli_percent);
```

- `init` waits for power-on, checks the calibration bit, and sends the
  initialization command when needed.
- `read` triggers a measurement, waits 80 ms, and validates the CRC.
- Temperature is millidegrees Celsius; humidity is millipercent RH.
- Returns 1 on success, 0 on a bus error, a busy sensor, or a bad CRC.

## Parameters

- I2C address `0x38`.
- Measurement time is about 80 ms.
- Conversion: `T = raw * 200 / 2^20 - 50`, `RH = raw * 100 / 2^20`.

## Memory and timing

- About 4 bytes; one 7-byte read.
- Do not trigger a new measurement before the previous one completes.

## Integration

```c
aht20_t sensor;
aht20_init(&sensor, board_i2c_port(), 0x38);
int32_t temperature, humidity;
if (aht20_read(&sensor, &temperature, &humidity)) {
    show(temperature / 1000, humidity / 1000);
}
```

## Adapters

- The adapter writes a three-byte command and reads seven bytes.
- Reference: LibDriver `aht20` (MIT) and the Aosong AHT20 datasheet.
- This implementation is original.

## When not to use

- Condensing environments.
- A sensor that must be read faster than 10 Hz.

## Verification

`make test` covers initialization, conversion, the busy status, and a bad CRC.
