# mcp2515

MCP2515 CAN controller over SPI.

## Application scenarios

- A CAN node on a vehicle or industrial bus.
- A diagnostic bridge between CAN and UART.
- A small sensor network with standard or extended frames.

## Contract

```c
typedef struct {
    uint32_t id;
    uint8_t extended;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

uint8_t mcp2515_init(mcp2515_t *device, mcp2515_port_t port,
                     uint8_t clock_mhz, uint16_t bitrate_kbps);
uint8_t mcp2515_send(mcp2515_t *device, const can_frame_t *frame);
uint8_t mcp2515_receive(mcp2515_t *device, can_frame_t *frame);
```

- `clock_mhz` is 8, 16, or 20; `bitrate_kbps` is 125, 250, 500, or 1000.
- `send` uses TX buffer 0.
- `receive` polls RX buffer 0/1 and clears the matching interrupt flag.
- Returns 1 on success or a received frame, 0 for no frame or an error.

## Parameters

- Bit timing uses the MCP2515 CNF1/CNF2/CNF3 tables for the chosen clock.
- RXB0 accepts all standard and extended frames with rollover enabled.
- A 120 ohm termination is required at each end of the bus.

## Memory and timing

- About 8 bytes plus the frame.
- `init` resets the controller and waits 10 ms.
- A frame write is a 13-byte SPI transfer; no dynamic allocation.

## Integration

```c
mcp2515_t can;
can_frame_t frame = {0x123, 0, 2, {0x11, 0x22}};
mcp2515_init(&can, board_mcp2515_port(), 16, 500);
mcp2515_send(&can, &frame);
if (mcp2515_receive(&can, &frame)) {
    handle(frame.id, frame.data, frame.dlc);
}
```

## Adapters

- The adapter drives CS low, transfers SPI bytes, and drives CS high.
- Reference: autowp/arduino-mcp2515 (MIT) and the Microchip MCP2515 datasheet.
- This implementation is original.

## When not to use

- A controller with a built-in CAN peripheral; use that instead.
- CAN FD; the MCP2515 supports classical CAN only.

## Verification

`make test` covers the reset and bit timing, a standard frame send, an extended
receive, and the empty-buffer path.
