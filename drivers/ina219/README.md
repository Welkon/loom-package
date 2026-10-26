# ina219

INA219 high-side current and power monitor.

## Application scenarios

- Battery or supply current logging.
- A motor or LED load monitor.
- A USB power budget check.

## Contract

```c
uint8_t ina219_init(ina219_t *device, ina219_port_t port, uint8_t i2c_address,
                    uint16_t shunt_milliohm, uint16_t current_lsb_ua);
uint8_t ina219_read_bus_voltage_mv(ina219_t *device, uint32_t *millivolts);
uint8_t ina219_read_shunt_voltage_uv(ina219_t *device, int32_t *microvolts);
uint8_t ina219_read_current_ua(ina219_t *device, int32_t *microamps);
uint8_t ina219_read_power_mw(ina219_t *device, uint32_t *milliwatts);
```

- `init` writes the 32 V / +-320 mV / 12-bit continuous configuration and
  the calibration register.
- Bus voltage LSB is 4 mV; shunt voltage LSB is 10 uV.
- Current LSB is the value passed to `init`; power LSB is 20 times that.
- Returns 1 on success, 0 on a bad argument or a bus error.

## Parameters

- I2C address `0x40`..`0x4F`.
- Shunt resistor in milliohms; current LSB in microamps.
- Keep the shunt voltage within the selected PGA range.

## Memory and timing

- 8 bytes; one 2-byte register write at init and one 2-byte read per value.
- Continuous conversion; a read returns the latest sample.

## Integration

```c
ina219_t monitor;
ina219_init(&monitor, board_i2c_port(), 0x40, 100, 100);
uint32_t millivolts;
ina219_read_bus_voltage_mv(&monitor, &millivolts);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: LibDriver `ina219` (MIT) and the Texas Instruments INA219 datasheet.
- This implementation is original.

## When not to use

- A high-side voltage beyond 32 V; use an isolated or higher-voltage monitor.
- A precision energy meter; add a dedicated metering IC.

## Verification

`make test` covers the config and calibration writes, bus/shunt/current/power
conversion, and range rejection.
