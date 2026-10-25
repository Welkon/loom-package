# ssd1306

Minimal SSD1306 128x64 I2C command and page-write driver.

## Application scenarios

- A status display for voltage, mode, or fault codes.
- A small menu on a product with one OLED.
- A bring-up display during manufacturing test.

## Contract

```c
typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t control, const uint8_t *data, uint16_t length);
} ssd1306_port_t;

void ssd1306_init(ssd1306_port_t port);
void ssd1306_clear(ssd1306_port_t port);
void ssd1306_set_contrast(ssd1306_port_t port, uint8_t value);
void ssd1306_write_page(ssd1306_port_t port, uint8_t page, uint8_t column,
                        const uint8_t *data, uint8_t width);
```

- `write` sends one I2C transaction: `control = 0x00` for commands, `0x40` for GDDRAM data.
- `init` sends the 128x64 charge-pump and addressing sequence.
- `clear` fills all eight pages with zeros.
- `write_page` writes one page (8 pixels high) from `column`.
- The caller owns the page buffer; no framebuffer or font is included.

## Parameters

- I2C address `0x3C` or `0x3D` is selected in the adapter.
- `width` must fit `column + width <= 128`.
- Contrast `0..255`; `0xCF` is a common start.

## Memory and timing

- 8 bytes of state plus the port context.
- `clear` sends 1024 data bytes; split into adapter-sized chunks.

## Integration

```c
ssd1306_port_t port = board_ssd1306_port();
ssd1306_init(port);
ssd1306_clear(port);
uint8_t row[128] = {0};
row[0] = 0x3C;
ssd1306_write_page(port, 2, 0, row, 128);
```

## Adapters

- Keep the I2C address, bus speed, and reset pin in the adapter.
- Reference: u8g2 `u8x8_d_ssd1306_128x64_noname.c` (BSD-2-Clause).
- The init sequence is hardware-specific; verify the panel's charge pump.

## When not to use

- A different controller (SH1106, SSD1309) needs a different init sequence.
- Text and graphics; add a font/renderer above this page API.

## Verification

`make test` covers the init command stream, clear pages, contrast, and a page write.
