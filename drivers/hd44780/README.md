# hd44780

HD44780-compatible character LCD, 4-bit parallel interface.

## Application scenarios

- A two-line status display.
- A menu or parameter editor.
- A legacy panel replacement.

## Contract

```c
uint8_t hd44780_init(hd44780_t *lcd, hd44780_port_t port, uint8_t columns, uint8_t rows);
void hd44780_clear(hd44780_t *lcd);
void hd44780_set_cursor(hd44780_t *lcd, uint8_t column, uint8_t row);
void hd44780_write_char(hd44780_t *lcd, char value);
void hd44780_write_string(hd44780_t *lcd, const char *text);
```

- The adapter owns RS, E, and D4..D7.
- `init` performs the 4-bit wake-up sequence for a 5x8 font.
- `set_cursor` accepts 1 or 2 rows and up to 20 columns.
- Returns 1 on success, 0 for an unsupported geometry.

## Parameters

- Columns 1..20; rows 1..2.
- The contrast and backlight are board concerns, not part of this unit.
- Use a 5 V-compatible level shifter when the MCU is 3.3 V.

## Memory and timing

- 4 bytes; 4-bit transfers, two pulses per byte.
- Clear needs about 2 ms; other commands need about 50 us.

## Integration

```c
hd44780_t lcd;
hd44780_init(&lcd, board_hd44780_port(), 16, 2);
hd44780_set_cursor(&lcd, 0, 0);
hd44780_write_string(&lcd, "loom");
```

## Adapters

- The adapter sets RS, writes D4..D7, and pulses E high then low.
- Reference: the HD44780 datasheet and the 4-bit sequence used by
  mathertel/LiquidCrystal_PCF8574 (MIT).
- This implementation is original.

## When not to use

- A graphic display; use an OLED or TFT driver.
- A long cable without buffering; the 4-bit bus is sensitive to noise.

## Verification

`make test` covers the wake-up nibbles, a character write, cursor addressing,
and geometry rejection.
