# clamp-debounce

Limit each step, then accept a value only after N stable samples.

## Contract

```c
void clamp_debounce_init(clamp_debounce_t *filter, uint16_t initial, uint16_t max_delta, uint16_t threshold);
uint16_t clamp_debounce_step(clamp_debounce_t *filter, uint16_t sample);
```

- `max_delta` caps the change from the accepted value.
- A clamped value must repeat `threshold` times before it is accepted.
- `threshold = 0` accepts immediately.
- No allocation.

## Use

Use it for a slow analog channel that must reject both spikes and
single-sample drift. Behavior reference: Padauk
`Soft_Filter/AD_filter_10`; this implementation is original.

## Avoid

Do not use it for a fast control loop; the clamp and the count add delay.
