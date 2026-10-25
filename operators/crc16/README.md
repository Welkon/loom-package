# crc16

CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, no final xor.

## Application scenarios

- A configuration block in flash must be checked before use.
- A firmware image or OTA chunk needs a compact integrity check.
- A UART or radio frame needs a checksum stronger than a sum.

## Contract

```c
uint16_t crc16_ccitt_update(uint16_t crc, uint8_t byte);
uint16_t crc16_ccitt(const uint8_t *data, size_t length);
```

- `crc16_ccitt` starts from `0xFFFF`.
- `crc16_ccitt_update` supports streamed input.
- Check value for `"123456789"` is `0x29B1`.
- Bitwise, no table, no allocation.

## Parameters

- This is the CCITT-FALSE variant; document it in the protocol.
- A table version trades flash for speed; not provided here.

## Memory and timing

- 2 bytes of state; 8 bit operations per byte.
- For large images, a table or hardware CRC may be faster.

## Integration

```c
uint16_t crc = crc16_ccitt(payload, length);
if (crc != expected) { reject(); }
```

## When not to use

- The peer uses Modbus, XMODEM, or another parameter set; use the matching variant.
- Cryptographic integrity is required; use a MAC or hash.

## Verification

`make test` covers the standard check value, streaming, and empty input.
