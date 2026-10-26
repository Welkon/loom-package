# mcp4725

MCP4725 12-bit I2C digital-to-analog converter.

## Application scenarios

- A 0..5 V setpoint for an analog circuit.
- Calibrating a sensor or actuator.
- A small waveform or bias source.

## Contract

```c
uint8_t mcp4725_init(mcp4725_t *device, mcp4725_port_t port, uint8_t i2c_address);
uint8_t mcp4725_write_value(mcp4725_t *device, uint16_t value);
```

- `value` is 0..4095; it updates the DAC register immediately.
- The output is `value / 4096 * VDD` (assuming VDD as reference).
- Returns 1 on success, 0 on a range error or a bus error.

## Parameters

- I2C address `0x60`..`0x67`.
- Power-down modes are not exposed; the normal mode is used.
- Keep the load within the output drive limit.

## Memory and timing

- 4 bytes; one 2-byte I2C write.
- The output settles in a few microseconds.

## Integration

```c
mcp4725_t dac;
mcp4725_init(&dac, board_i2c_port(), 0x60);
mcp4725_write_value(&dac, 2048); /* about half scale */
```

## Adapters

- The adapter issues the two-byte DAC command.
- Reference: LibDriver `mcp4725` (MIT) and the Microchip MCP4725 datasheet.
- This implementation is original.

## When not to use

- A PWM plus RC filter is sufficient.
- A current-output loop; use a current DAC.

## Verification

`make test` covers a mid-scale write, full scale, zero, and range rejection.
