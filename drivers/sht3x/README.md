# sht3x

SHT30/SHT31/SHT35 temperature and humidity sensor.

## Application scenarios

- Room climate monitoring.
- Greenhouse or enclosure humidity.
- A calibrated replacement for a DHT sensor.

## Contract

```c
typedef enum {
    SHT3X_HIGH = 0,
    SHT3X_MEDIUM,
    SHT3X_LOW,
} sht3x_repeatability_t;

uint8_t sht3x_init(sht3x_t *device, sht3x_port_t port, uint8_t i2c_address);
uint8_t sht3x_read(sht3x_t *device, sht3x_repeatability_t repeatability,
                   int32_t *milli_celsius, int32_t *milli_percent);
```

- `read` sends a single-shot command, waits, reads six bytes, and checks both CRCs.
- Temperature is millidegrees Celsius; humidity is millipercent RH.
- Returns 1 on success, 0 on a bus error, a busy sensor, or a bad CRC.

## Parameters

- I2C address `0x44` or `0x45`.
- Repeatability high/medium/low changes the measurement time and noise.
- Conversion: `T = -45 + 175 * raw / 65535`,
  `RH = 100 * raw / 65535`.

## Memory and timing

- About 8 bytes; one 6-byte read.
- Measurement takes about 15 ms (high), 6 ms (medium), or 4 ms (low).

## Integration

```c
sht3x_t sensor;
sht3x_init(&sensor, board_i2c_port(), 0x44);
int32_t temperature, humidity;
if (sht3x_read(&sensor, SHT3X_HIGH, &temperature, &humidity)) {
    show(temperature / 1000, humidity / 1000);
}
```

## Adapters

- The adapter writes two command bytes and reads six data bytes.
- Reference: DFRobot_SHT3x (MIT) and the Sensirion SHT3x datasheet.
- This implementation is original.

## When not to use

- A sensor on a long cable without a buffer.
- Condensing environments; use a protected sensor.

## Verification

`make test` covers both CRCs, the conversion, repeatability commands, and a
bad CRC.
