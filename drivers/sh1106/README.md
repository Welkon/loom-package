# sh1106

Minimal SH1106 128x64 I2C OLED driver.

## Application scenarios

- A 1.3-inch status display.
- A menu on a handheld instrument.
- A bring-up display for a new board.

## Contract

```c
void sh1106_init(sh1106_port_t port);
void sh1106_clear(sh1106_port_t port);
void sh1106_set_contrast(sh1106_port_t port, uint8_t value);
void sh1106_write_page(sh1106_port_t port, uint8_t page, uint8_t column,
                       const uint8_t *data, uint8_t width);
```

- `write` sends commands with control 0x00 and data with control 0x40.
- `init` sends the SH1106 DC-DC, timing, and COM sequence.
- `clear` fills all eight pages with zeros.
- `write_page` writes one page (8 pixels) at `column`.
- The caller owns the page buffer; no framebuffer or font is included.

## Parameters

- I2C address `0x3C` or `0x3D` is selected in the adapter.
- `width` must fit `column + width <= 128`.
- Contrast `0..255`; `0xFF` is a common start.

## Memory and timing

- 8 bytes of state plus the port context.
- `clear` sends 1024 data bytes; split into adapter-sized chunks.
- SH1106 has 132 columns; visible RAM starts at column 2.

## Integration

```c
sh1106_port_t port = board_sh1106_port();
sh1106_init(port);
sh1106_clear(port);
uint8_t row[128] = {0};
row[0] = 0x3C;
sh1106_write_page(port, 2, 0, row, 128);
```

## Adapters

- The adapter handles the I2C address, bus speed, and reset pin.
- Reference: u8g2 `u8x8_d_ssd1306_128x64_noname.c` SH1106 sequence (BSD-2-Clause).
- Verify the panel's DC-DC and COM configuration.

## When not to use

- An SSD1306 panel; use the `ssd1306` unit.
- Text and graphics; add a font/renderer above this page API.

## Verification

`make test` covers the init sequence, clear, contrast, and a page write with
the SH1106 column offset.
