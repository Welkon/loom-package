# ir-nec-decode

Decode NEC IR falling-edge intervals into an address and command.

## Application scenarios

- A TV or appliance remote sends NEC frames to an IR receiver.
- A product uses a cheap IR remote for setup or service.
- An IR keypad sends address/command pairs.

## Contract

```c
typedef enum { IR_NEC_INVALID = 0, IR_NEC_FRAME = 1, IR_NEC_REPEAT = 2 } ir_nec_result_t;
typedef struct { uint8_t address; uint8_t command; } ir_nec_frame_t;
ir_nec_result_t ir_nec_decode(const uint16_t *intervals_us, uint8_t count, ir_nec_frame_t *frame);
```

- Input is falling-edge intervals: one leader, then 32 bit intervals (MSB first).
- Leader 12..16 ms; bit 0 0.7..1.5 ms; bit 1 1.8..2.8 ms.
- Address and command complements must match.
- A single 10..12 ms interval is a repeat frame.
- No allocation; O(1) for 33 intervals.

## Parameters

- The tolerance windows match common 38 kHz receivers; verify against the chosen part.
- The capture port must time falling edges with microsecond resolution.

## Memory and timing

- No state; the caller provides 33 intervals (66 bytes at 16-bit).
- O(1); no division.

## Integration

```c
ir_nec_frame_t frame;
if (ir_nec_decode(intervals, 33, &frame) == IR_NEC_FRAME) {
    handle_key(frame.address, frame.command);
}
```

## When not to use

- Raw GPIO levels; a capture port must produce edge intervals.
- RC5, Sony, or other protocols; use the matching decoder.

## Verification

`make test` covers a valid frame, bad bit, bad complement, repeat, and wrong count.
Behavior reference: the NEC protocol and a vendor IR receiver example.
