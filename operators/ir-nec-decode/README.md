# ir-nec-decode

Decode NEC IR falling-edge intervals into an address and command.

## Contract

```c
typedef enum {
    IR_NEC_INVALID = 0,
    IR_NEC_FRAME = 1,
    IR_NEC_REPEAT = 2
} ir_nec_result_t;

typedef struct {
    uint8_t address;
    uint8_t command;
} ir_nec_frame_t;

ir_nec_result_t ir_nec_decode(const uint16_t *intervals_us, uint8_t count, ir_nec_frame_t *frame);
```

- Input is falling-edge intervals in microseconds: one leader interval,
  then 32 bit intervals (MSB first).
- Leader is 12..16 ms; bit 0 is 0.7..1.5 ms; bit 1 is 1.8..2.8 ms.
- Address and command complements must match.
- A single 10..12 ms interval is a repeat frame.
- No allocation; cost is O(1) for 33 intervals.

## Use

Use it after a capture port produced edge timestamps. Behavior reference:
a production target `ir.c` and Padauk `IR/IR_Receive`; this implementation is original.

## Avoid

Do not decode raw GPIO levels; the caller must provide edge intervals.
Other protocols (RC5, Sony) need their own decoder.
