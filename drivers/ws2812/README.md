# ws2812

Encode RGB pixels into the GRB byte order used by WS2812 strips.

## Application scenarios

- A single WS2812 status LED on a product.
- A short LED strip for state, battery, or progress.
- A Padauk/CA51 project that already has a bit-banged WS2812 example.

## Contract

```c
typedef struct {
    void *context;
    void (*write_byte)(void *context, uint8_t byte);
    void (*reset)(void *context);
} ws2812_port_t;

void ws2812_init(ws2812_t *strip, ws2812_port_t port);
void ws2812_send_rgb(ws2812_t *strip, const uint8_t *rgb, uint16_t count);
void ws2812_send_grb(ws2812_t *strip, const uint8_t *grb, uint16_t count);
```

- `rgb` is three bytes per pixel: red, green, blue.
- `write_byte` sends one byte MSB first with the WS2812 bit timing.
- `reset` holds the line low for at least 50 us.
- No allocation.

## Parameters

- The port owns the bit timing and the CPU clock dependency.
- `count` is the number of pixels; the buffer is `3*count` bytes.

## Memory and timing

- 8 bytes of state; O(count) bytes.
- Bit timing is in the adapter; the core only orders GRB bytes.

## Integration

```c
const uint8_t red[3] = {255, 0, 0};
ws2812_t strip;
ws2812_init(&strip, board_ws2812_port());
ws2812_send_rgb(&strip, red, 1);
```

## Adapters

- Keep the timer, GPIO, interrupt masking, and reset timing in the adapter.
- Reference example: Padauk `Module_Sensor/WS2812B`.
- Do not copy vendor delay loops into the core.

## When not to use

- APA102 or other clocked strips; they need a different protocol.
- Very long strips where DMA is required; the port can still implement DMA,
  but the core remains byte-oriented.

## Verification

`make test` covers GRB ordering, multiple pixels, GRB passthrough, and reset.
