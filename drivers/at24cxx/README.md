# at24cxx

AT24Cxx I2C EEPROM driver.

## Application scenarios

- Calibration constants and serial numbers.
- User settings that survive power loss.
- A small event log with page writes.

## Contract

```c
typedef struct {
    void *context;
    uint8_t (*write_at)(void *context, uint8_t address, uint32_t memory,
                        const uint8_t *data, uint16_t length);
    uint8_t (*read_at)(void *context, uint8_t address, uint32_t memory,
                       uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} at24cxx_port_t;

uint8_t at24cxx_init(at24cxx_t *device, at24cxx_port_t port,
                     uint8_t i2c_address, at24cxx_type_t type);
uint8_t at24cxx_read(at24cxx_t *device, uint32_t address,
                     uint8_t *data, uint16_t length);
uint8_t at24cxx_write(at24cxx_t *device, uint32_t address,
                      const uint8_t *data, uint16_t length);
```

- `write_at` and `read_at` perform one I2C transaction with the memory address.
- `write` splits at page boundaries and waits 5 ms after each page.
- Returns 1 on success, 0 on a range error or a bus error.
- Supported types: 24C01, 24C02, 24C04, 24C08, 24C16, 24C32, 24C64,
  24C128, 24C256, 24C512.

## Parameters

- Page size is 8, 16, 32, 64, or 128 bytes depending on the type.
- 24C01..24C16 use a one-byte memory address; larger parts use two.
- The I2C address is 7-bit (`0x50`..`0x57`).

## Memory and timing

- No allocation. `write` needs one page delay per page (5 ms).
- A full 24C512 write takes about 2.6 s; do it outside a control loop.
- Read has no page limit; the adapter may cap the transfer length.

## Integration

```c
at24cxx_t eeprom;
at24cxx_init(&eeprom, board_i2c_port(), 0x50, AT24C02);
uint8_t serial[8];
at24cxx_read(&eeprom, 0x00, serial, sizeof(serial));
```

## Adapters

- The adapter adds the memory address bytes and issues a repeated start.
- Reference: LibDriver `at24cxx` (MIT) and the Atmel AT24Cxx datasheet.
- This implementation is original.

## When not to use

- More than 64 KiB; use a flash chip or a file system.
- Frequent writes; EEPROM endurance is limited.

## Verification

`make test` covers a page-boundary write, read-back, range rejection, and
the page delay count.
