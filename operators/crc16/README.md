# crc16

CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, no final xor.

## Contract

```c
uint16_t crc16_ccitt_update(uint16_t crc, uint8_t byte);
uint16_t crc16_ccitt(const uint8_t *data, size_t length);
```

- `crc16_ccitt` starts from `0xFFFF`.
- `crc16_ccitt_update` allows streamed input.
- The check value for `"123456789"` is `0x29B1`.
- Bitwise, no table, no allocation.

## Use

Use it for configuration blocks, firmware images, and serial frames.

## Avoid

Do not mix this variant with Modbus or XMODEM; they use different
parameters. Name the variant in the protocol.
