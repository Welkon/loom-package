# rotary-encoder

Quadrature decoder for a mechanical rotary encoder.

## Application scenarios

- A menu knob on an instrument.
- A setpoint or volume control.
- A position readout on a manual stage.

## Contract

```c
typedef struct {
    uint8_t state;
    int16_t position;
} encoder_t;

void encoder_init(encoder_t *encoder);
int8_t encoder_update(encoder_t *encoder, uint8_t a, uint8_t b);
int16_t encoder_position(const encoder_t *encoder);
```

- `a` and `b` are the two switch levels, 0 or 1.
- `update` returns +1 or -1 for a valid edge, 0 for no change or an invalid jump.
- `position` accumulates every valid edge; divide by four for one detent.
- Call at a fixed rate or from a pin-change interrupt.

## Parameters

- Mechanical encoders often produce four edges per detent.
- Add an RC filter or sample debounce when the encoder is noisy.
- Reverse the sign in the adapter if the direction is inverted.

## Memory and timing

- 4 bytes; O(1); no allocation.

## Integration

```c
encoder_t knob;
encoder_init(&knob);
int8_t step = encoder_update(&knob, read_a(), read_b());
if (step != 0) {
    menu_move(step);
}
```

## Adapters

- Read the two GPIO pins with pull-ups.
- Reference: PaulStoffregen/Encoder (MIT) state-table behavior.
- This implementation is original.

## When not to use

- A motor encoder with an index or high count; use a hardware QEP.
- A noisy line without filtering; debounce first.

## Verification

`make test` covers a full clockwise cycle, counter-clockwise, invalid jumps,
and a bounce sequence.
