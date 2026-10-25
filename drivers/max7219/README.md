# max7219

MAX7219/MAX7221 SPI LED driver.

## Application scenarios

- An eight-digit seven-segment display.
- An 8x8 LED matrix or a daisy chain of matrices.
- A bar graph or status panel.

## Contract

```c
typedef struct {
    void *context;
    void (*select)(void *context);
    uint8_t (*transfer)(void *context, uint8_t value);
    void (*deselect)(void *context);
} max7219_port_t;

void max7219_init(max7219_t *driver, max7219_port_t port, uint8_t devices);
void max7219_write(max7219_t *driver, uint8_t device, uint8_t reg, uint8_t value);
void max7219_set_digit(max7219_t *driver, uint8_t device, uint8_t digit, uint8_t value);
void max7219_set_row(max7219_t *driver, uint8_t device, uint8_t row, uint8_t value);
void max7219_set_intensity(max7219_t *driver, uint8_t device, uint8_t value);
void max7219_shutdown(max7219_t *driver, uint8_t device, uint8_t shutdown);
void max7219_clear(max7219_t *driver, uint8_t device);
```

- `devices` is 1..8; device 0 is the nearest to the controller.
- `write` sends a 16-bit frame to one device and no-op frames to the rest.
- `digit` and `row` are 0..7; `value` is 0..15 (raw segments/rows).
- `shutdown` 0 enables the display, 1 blanks it.

## Parameters

- Registers: decode 0x09, intensity 0x0A, scan 0x0B, shutdown 0x0C.
- Use decode mode 0 for raw segments or matrix rows.
- Intensity 0..15.

## Memory and timing

- 4 bytes; each frame is 16 bits; a chain write takes `devices` frames.
- The SPI clock can run up to 10 MHz.

## Integration

```c
max7219_t display;
max7219_init(&display, board_max7219_port(), 2);
max7219_clear(&display, 0);
max7219_shutdown(&display, 0, 0);
max7219_set_digit(&display, 0, 0, 0x0F);
```

## Adapters

- The adapter drives CS low, transfers two bytes MSB first, and drives CS high.
- Reference: wayoda/LedControl (MIT) and the MAX7219 datasheet.
- This implementation is original.

## When not to use

- A driver with built-in font decoding; use a display controller.
- More than eight devices; add a second CS line.

## Verification

`make test` covers the init sequence, chain order, digit writes, intensity,
and shutdown.
