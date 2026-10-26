# ads1115

ADS1115 16-bit I2C analog-to-digital converter.

## Application scenarios

- A precision sensor with a small analog output.
- Battery or supply voltage measurement.
- A bridge or thermistor front end.

## Contract

```c
typedef enum {
    ADS1115_RANGE_6P144V = 0,
    ADS1115_RANGE_4P096V,
    ADS1115_RANGE_2P048V,
    ADS1115_RANGE_1P024V,
    ADS1115_RANGE_0P512V,
    ADS1115_RANGE_0P256V,
} ads1115_range_t;

uint8_t ads1115_init(ads1115_t *device, ads1115_port_t port, uint8_t i2c_address);
uint8_t ads1115_read_single(ads1115_t *device, uint8_t channel,
                            ads1115_range_t range, int16_t *raw, int32_t *microvolts);
```

- `channel` is 0..3 (single-ended AIN0..AIN3).
- `read_single` starts a single conversion, waits 10 ms, and reads the result.
- `microvolts` is the signed input voltage in microvolts.
- Returns 1 on success, 0 on a bad argument or a bus error.

## Parameters

- I2C address `0x48`..`0x4B` from ADDR.
- Range sets the full scale; the LSB is `range / 32768`.
- The input must stay within the selected range.

## Memory and timing

- 4 bytes; one 2-byte config write and one 2-byte conversion read.
- 128 samples/s; a single read takes about 8 ms.

## Integration

```c
ads1115_t adc;
int16_t raw;
int32_t microvolts;
ads1115_init(&adc, board_i2c_port(), 0x48);
ads1115_read_single(&adc, 0, ADS1115_RANGE_4P096V, &raw, &microvolts);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: LibDriver `ads1115` (MIT) and the Texas Instruments ADS1115 datasheet.
- This implementation is original.

## When not to use

- A ratiometric MCU ADC is enough; use the internal ADC.
- Differential measurements with a strict common-mode limit; check the datasheet.

## Verification

`make test` covers the config word, positive and negative conversion,
range scaling, and channel validation.
