# sdcard

SD card block device over SPI.

## Application scenarios

- A data logger with a removable card.
- A firmware image or configuration store.
- A low-cost file system underneath FatFs.

## Contract

```c
uint8_t sdcard_init(sdcard_t *card, sdcard_port_t port);
uint8_t sdcard_read_block(sdcard_t *card, uint32_t block, uint8_t data[512]);
uint8_t sdcard_write_block(sdcard_t *card, uint32_t block, const uint8_t data[512]);
```

- `init` performs the SPI wake-up, CMD0, CMD8, ACMD41, CMD58, and CMD16.
- Blocks are 512 bytes; `block` is a logical block address.
- Returns 1 on success, 0 on a timeout or a bad response.

## Parameters

- SPI mode 0, 400 kHz during init, then up to 25 MHz.
- 3.3 V card supply; level-shift if the MCU is 5 V.
- CMD17/CMD24 only; partitioning and FAT are file-system layers.

## Memory and timing

- 8 bytes; the caller owns one 512-byte block buffer.
- Init can wait up to about 1 s for ACMD41; a block read/write takes a few ms.

## Integration

```c
sdcard_t card;
uint8_t block[512];
sdcard_init(&card, board_sdcard_port());
if (sdcard_read_block(&card, 0, block)) {
    parse_boot_sector(block);
}
```

## Adapters

- The adapter drives CS, SCK, MOSI, MISO, and a 1 ms delay.
- Reference: the SD Physical Layer Simplified Specification and the
  Yanye0xFF/ESP8266-SPI-SDCARD implementation (Apache-2.0).
- This implementation is original.

## When not to use

- eMMC or SDIO high-speed modes; use a dedicated host controller.
- A card that needs partitioning; add a file-system layer above this block API.

## Verification

`make test` covers the init command sequence, a block read, a block write,
CRC token handling, and a missing data token.
