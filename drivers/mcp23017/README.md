# mcp23017

MCP23017 16-bit I2C GPIO expander.

## Application scenarios

- Extra buttons, relays, or LEDs on a small MCU.
- A keypad or DIP switch bank.
- Isolating a slow control panel from the main board.

## Contract

```c
uint8_t mcp23017_init(mcp23017_t *device, mcp23017_port_t port, uint8_t i2c_address);
uint8_t mcp23017_pin_mode(mcp23017_t *device, uint8_t pin, uint8_t output);
uint8_t mcp23017_write_pin(mcp23017_t *device, uint8_t pin, uint8_t value);
uint8_t mcp23017_read_pin(mcp23017_t *device, uint8_t pin, uint8_t *value);
uint8_t mcp23017_write_port(mcp23017_t *device, uint8_t port, uint8_t value);
uint8_t mcp23017_read_port(mcp23017_t *device, uint8_t port, uint8_t *value);
uint8_t mcp23017_set_pullup(mcp23017_t *device, uint8_t pin, uint8_t enable);
```

- Pins 0..7 are port A; pins 8..15 are port B.
- `init` uses IOCON.BANK=0 and sets all pins as inputs with pull-ups off.
- `write_pin` changes one output latch bit; other bits keep their state.
- Returns 1 on success, 0 on a bad pin or a bus error.

## Parameters

- I2C address `0x20`..`0x27` from A2/A1/A0.
- `pin_mode` 0 = input, 1 = output.
- Pull-ups are useful for buttons; disable them for driven inputs.

## Memory and timing

- 4 bytes; one or two I2C transactions per call.
- A 400 kHz bus updates a pin in about 60 us.

## Integration

```c
mcp23017_t expander;
mcp23017_init(&expander, board_i2c_port(), 0x20);
mcp23017_pin_mode(&expander, 0, 1);
mcp23017_write_pin(&expander, 0, 1);
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: blemasle/arduino-mcp23017 (MIT) and the Microchip MCP23017 datasheet.
- This implementation is original.

## When not to use

- A pin that needs a hardware timer, ADC, or interrupt latency; use the MCU.
- More than 16 pins; use multiple expanders.

## Verification

`make test` covers init, pin direction, output latch updates, input reads,
ports, and pull-ups.
