# crc32

CRC-32/ISO-HDLC (the zlib/Ethernet variant).

## Application scenarios

- Configuration blocks and firmware images.
- File or frame integrity where CRC-16 is too small.
- Compatibility with zlib, gzip, PNG, or Ethernet tools.

## Contract

```c
uint32_t crc32_update(uint32_t crc, uint8_t byte);
uint32_t crc32(const uint8_t *data, size_t length);
```

- Polynomial `0xEDB88320` (reflected `0x04C11DB7`).
- Init `0xFFFFFFFF`; final xor `0xFFFFFFFF`.
- Check value for `"123456789"` is `0xCBF43926`.
- Bitwise, no table, no allocation; `crc32_update` supports streaming.

## Parameters

- This is the zlib variant; document it in the protocol.
- A 256-entry table trades 1 KB flash for speed.

## Memory and timing

- 4 bytes of state; 8 XOR/shift steps per byte.

## Integration

```c
uint32_t crc = crc32(payload, length);
if (crc != expected) { reject(); }
```

## When not to use

- A cryptographic check is required; use a MAC or hash.
- The peer uses CRC-32C (Castagnoli) or CRC-32/BZIP2; use the matching variant.

## Verification

`make test` covers the standard check value, streaming, and empty input.
Behavior reference: zlib `crc32.c` (zlib license); this implementation is original.
