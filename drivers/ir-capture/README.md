# ir-capture

Collect falling-edge intervals for an IR receiver.

## Application scenarios

- A NEC remote receiver on an EXTI input.
- An IR keypad that needs raw intervals for a protocol decoder.
- A capture path shared by several IR protocols.

## Contract

```c
typedef struct {
    uint16_t intervals[33];
    uint8_t count;
    uint32_t last_us;
    uint8_t active;
    uint8_t ready;
} ir_capture_t;

void ir_capture_init(ir_capture_t *capture);
void ir_capture_reset(ir_capture_t *capture);
void ir_capture_edge(ir_capture_t *capture, uint32_t now_us);
uint8_t ir_capture_ready(const ir_capture_t *capture);
uint8_t ir_capture_take(ir_capture_t *capture, uint16_t *intervals, uint8_t max_count);
```

- Call `ir_capture_edge` from the falling-edge ISR with a microsecond timestamp.
- The first edge starts a frame; subsequent edges store intervals.
- A gap above 30 ms starts a new frame.
- `ir_capture_take` copies 33 intervals and resets; it returns 0 if not ready.
- No allocation.

## Parameters

- The interval array is sized for one NEC frame (leader + 32 bits).
- The 30 ms gap threshold is a compile-time constant; adjust for the protocol.

## Memory and timing

- 74 bytes of state; O(1) per edge.
- The ISR does one subtraction and one store.

## Integration

```c
ir_capture_t capture;
ir_capture_init(&capture);
/* EXTI ISR: */ ir_capture_edge(&capture, board_now_us());

uint16_t intervals[33];
if (ir_capture_take(&capture, intervals, 33) == 33) {
    ir_nec_frame_t frame;
    if (ir_nec_decode(intervals, 33, &frame) == IR_NEC_FRAME) {
        handle_key(frame.address, frame.command);
    }
}
```

## Adapters

- Keep EXTI setup, pull-up, and timer selection in the adapter.
- Feed a free-running microsecond counter; 16-bit counters wrap and need widening.
- Do not decode inside the ISR; take the frame in the main loop.

## When not to use

- IR protocols with a different frame length; change the array size or use a
  protocol-specific capture.
- A receiver with an integrated decoder that already outputs a key code.

## Verification

`make test` covers frame collection, gap restart, take/reset, and short frames.
