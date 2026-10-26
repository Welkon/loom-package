# mcp9808

MCP9808 high-accuracy I2C temperature sensor.

## Application scenarios

- Board or enclosure temperature.
- A calibrated replacement for an NTC.
- Thermal logging near a power stage.

## Contract

```c
uint8_t mcp9808_init(mcp9808_t *device, mcp9808_port_t port, uint8_t i2c_address);
uint8_t mcp9808_read(mcp9808_t *device, int32_t *milli_celsius);
```

- `init` sets +-0.0625 degrees C resolution and continuous conversion.
- `read` returns millidegrees Celsius.
- Returns 1 on success, 0 on a bus error.

## Parameters

- I2C address `0x18`..`0x1F`.
- Resolution 0.0625 degrees C.
- The alert output is not exposed; use the threshold registers in an adapter.

## Memory and timing

- 4 bytes; one 2-byte read.
- A conversion takes about 250 ms at the highest resolution.

## Integration

```c
mcp9808_t sensor;
mcp9808_init(&sensor, board_i2c_port(), 0x18);
int32_t temperature;
mcp9808_read(&sensor, &temperature);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: LibDriver `mcp9808` (MIT) and the Microchip MCP9808 datasheet.
- This implementation is original.

## When not to use

- A remote probe several meters away; use a digital remote sensor.
- A fast control loop; the conversion time is too long.

## Verification

`make test` covers positive, negative, and fractional temperatures.
