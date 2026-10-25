# uart

Blocking UART core with a port and timeout.

## Application scenarios

- A GPS, Bluetooth, or cellular module that accepts AT commands.
- A debug console that writes bytes and reads a line.
- A service port for calibration or manufacturing test.

## Contract

```c
typedef struct {
    void *context;
    void (*write)(void *context, uint8_t byte);
    uint8_t (*read_ready)(void *context);
    uint8_t (*read)(void *context);
    uint32_t (*now_us)(void *context);
} uart_port_t;

void uart_init(uart_t *uart, uart_port_t port, uint32_t timeout_us);
void uart_write(uart_t *uart, const uint8_t *data, uint16_t length);
uart_status_t uart_read(uart_t *uart, uint8_t *data, uint16_t length);
```

- `timeout_us = 0` waits forever; use it only when the peer is guaranteed.
- `read_ready` returns 1 when a byte is available.
- The port owns baud rate, FIFO, and error handling.
- No allocation.

## Parameters

- `timeout_us`: worst-case response time; 10..1000 ms depending on the peer.
- For interrupt-driven RX, use the `ring-buffer` operator in the ISR and a
  non-blocking poll in the main loop.

## Memory and timing

- 24 bytes of state plus the port context.
- O(N) bytes; one busy poll per byte.

## Integration

```c
uart_t link;
uart_init(&link, board_uart_port(), 100000);
uart_write(&link, (const uint8_t *)"AT\r\n", 4);
uint8_t reply[2];
if (uart_read(&link, reply, 2) == UART_OK) { handle(reply); }
```

## Adapters

- Keep baud rate, pin mux, FIFO, and error flags in the adapter.
- Reference examples: a production target, a legacy demo `ca51m550`, a production target UART demos.
- Do not copy a vendor register sequence into the core.

## When not to use

- High-throughput streaming; use DMA plus the `ring-buffer` operator.
- Multi-drop RS-485 without an external transceiver control layer.

## Verification

`make test` covers write order, immediate read, timeout, and zero timeout.
