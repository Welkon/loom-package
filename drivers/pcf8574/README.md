# pcf8574

PCF8574 8-bit I2C GPIO expander.

## Application scenarios

- A few buttons or relays on a small MCU.
- A status LED bank.
- A slow control panel.

## Contract

```c
uint8_t pcf8574_init(pcf8574_t *device, pcf8574_port_t port, uint8_t i2c_address);
uint8_t pcf8574_write_port(pcf8574_t *device, uint8_t value);
uint8_t pcf8574_read_port(pcf8574_t *device, uint8_t *value);
uint8_t pcf8574_write_pin(pcf8574_t *device, uint8_t pin, uint8_t value);
uint8_t pcf8574_read_pin(pcf8574_t *device, uint8_t pin, uint8_t *value);
```

- Pins are 0..7.
- A written 1 releases the pin; an external driver can then pull it low.
- `write_pin` updates a shadow latch, so other output pins keep their state.
- Returns 1 on success, 0 on a bad pin or a bus error.

## Parameters

- I2C address `0x20`..`0x27` from A2/A1/A0.
- There is no direction register; use open-drain drive or a series resistor.
- The reset state is all ones (inputs released).

## Memory and timing

- 4 bytes; one I2C write or read.
- A pin update is one byte on the bus.

## Integration

```c
pcf8574_t expander;
pcf8574_init(&expander, board_i2c_port(), 0x20);
pcf8574_write_pin(&expander, 0, 0); /* drive low */
pcf8574_write_pin(&expander, 0, 1); /* release */
```

## Adapters

- The adapter writes or reads one byte, with no register pointer.
- Reference: LibDriver `pcf8574` (MIT) and the Texas Instruments PCF8574 datasheet.
- This implementation is original.

## When not to use

- A pin that needs a direction register or interrupt masking; use MCP23017.
- A fast signal; the I2C update rate is the limit.

## Verification

`make test` covers init, port writes, pin updates, reads, and invalid pins.
