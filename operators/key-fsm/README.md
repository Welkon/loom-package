# key-fsm

Turn a raw key level into debounced short and long events.

## Contract

```c
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT = 1,
    KEY_EVENT_LONG = 2
} key_event_t;

void key_fsm_init(key_fsm_t *fsm, uint16_t debounce_ms, uint16_t long_ms);
key_event_t key_fsm_step(key_fsm_t *fsm, uint8_t raw, uint16_t elapsed_ms);
```

- Call on a fixed tick with the elapsed milliseconds.
- A press is accepted after `debounce_ms`.
- A long event is emitted once after `long_ms` while held.
- A release before the long threshold emits one short event.
- A release after a long event emits none.
- No allocation.

## Use

Use it for a single key with short/long actions. Behavior reference:
a production target `key.c` and a production target key modes; this implementation is original.

## Avoid

Do not use it for a matrix or multi-touch. Build those on `debounce` with
a scan layer.
