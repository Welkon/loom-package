# key-fsm

Turn a raw key level into debounced short and long events.

## Application scenarios

- One power button with short press (on/off) and long press (reset or pairing).
- A single-key menu: short press advances, long press enters setup.
- A device with no room for a key matrix.

## Contract

```c
typedef enum { KEY_EVENT_NONE = 0, KEY_EVENT_SHORT = 1, KEY_EVENT_LONG = 2 } key_event_t;
void key_fsm_init(key_fsm_t *fsm, uint16_t debounce_ms, uint16_t long_ms);
key_event_t key_fsm_step(key_fsm_t *fsm, uint8_t raw, uint16_t elapsed_ms);
```

- Call on a fixed tick with elapsed milliseconds.
- Press is accepted after `debounce_ms`.
- Long event is emitted once after `long_ms` while held.
- Release before the long threshold emits one short event.
- Release after a long event emits none.
- No allocation.

## Parameters

- `debounce_ms`: 10..30 ms for a mechanical key.
- `long_ms`: 600..1500 ms; longer avoids accidental long presses.

## Memory and timing

- 12 bytes of state; O(1).

## Integration

```c
key_fsm_t key;
key_fsm_init(&key, 20, 1000);
/* every 1 ms: */
switch (key_fsm_step(&key, raw_pin(), 1)) {
case KEY_EVENT_SHORT: toggle_output(); break;
case KEY_EVENT_LONG: enter_setup(); break;
default: break;
}
```

## When not to use

- A key matrix or multi-touch; build a scan layer and use `debounce` per key.
- The caller cannot provide a stable time base.

## Verification

`make test` covers short, long, release after long, bounce, and zero timing.
Behavior reference: production key modes; this implementation is original.
