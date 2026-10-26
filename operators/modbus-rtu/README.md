# modbus-rtu

Modbus RTU frame builder and parser.

## Application scenarios

- A UART sensor or drive on an RS-485 bus.
- A gateway between Modbus and a local protocol.
- A bench tool that checks slave responses.

## Contract

```c
uint16_t modbus_crc16(const uint8_t *data, uint16_t length);
uint8_t modbus_build_read(uint8_t slave, uint8_t function,
                          uint16_t address, uint16_t count, uint8_t *frame);
uint8_t modbus_parse_read(const uint8_t *frame, uint16_t length,
                          uint8_t slave, uint8_t function,
                          uint16_t *values, uint8_t capacity, uint8_t *count);
uint8_t modbus_parse_exception(const uint8_t *frame, uint16_t length,
                               uint8_t slave, uint8_t function, uint8_t *code);
```

- `modbus_build_read` builds an 8-byte request with the CRC appended low
  byte first. It supports function 0x03 and counts 1..125.
- `modbus_parse_read` validates slave, function, byte count, and CRC, then
  decodes big-endian 16-bit registers.
- `modbus_parse_exception` accepts the 5-byte exception response.
- Returns 1 on success, 0 on a malformed frame.

## Parameters

- Slave address 1..247; broadcast 0 is not a read request.
- Function 0x03 (holding registers); other functions use the same frame shape.
- UART settings are project-specific: 8N1, 9600/19200/115200, and a timeout.

## Memory and timing

- No allocation; the caller owns the frame.
- A request is 8 bytes; a response is `5 + 2 * count` bytes.
- A 3.5-character silent interval delimits frames; the UART adapter owns it.

## Integration

```c
uint8_t frame[8];
uint16_t registers[4];
uint8_t count;
modbus_build_read(1, 0x03, 0x0000, 4, frame);
/* send frame, receive response into frame */
if (modbus_parse_read(frame, response_length, 1, 0x03, registers, 4, &count)) {
    use(registers, count);
}
```

## Adapters

- The adapter owns UART framing, DE/RE for RS-485, and the inter-frame timeout.
- Reference: Modbus over Serial Line Specification V1.02 and the
  `emelianov/modbus-esp8266` frame behavior (MIT).
- This implementation is original.

## When not to use

- Modbus TCP; use an MBAP header layer.
- A bus without a defined turnaround or termination.

## Verification

`make test` covers the CRC vector, request bytes, a valid response, an
exception, and malformed frames.
