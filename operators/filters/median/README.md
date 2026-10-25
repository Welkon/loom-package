# median

Return the median of the last N samples.

## Application scenarios

- PIR or ADC samples contain isolated impulses.
- A key or limit switch produces contact bounce.
- A battery reading is disturbed by a load pulse.

## Contract

```c
void median_filter_init(median_filter_t *filter, uint16_t *window, uint8_t size);
uint16_t median_filter_step(median_filter_t *filter, uint16_t sample);
void median_filter_reset(median_filter_t *filter);
```

- The caller supplies `window`, an array of `size` elements.
- `size` must be 1..255. Odd sizes give the true median; even sizes return the lower middle value.
- No scratch buffer and no allocation.
- Cost is O(N^2) per sample; use small N (3, 5, or 7).

## Parameters

- Odd `size`; a larger window removes more impulses but adds delay of about `(size-1)/2` samples.
- `window` must remain valid for the filter lifetime.

## Memory and timing

- State is 4 bytes plus the caller's `2*size` bytes.
- O(N^2) comparisons; at N=7 this is 49 comparisons.

## Integration

```c
uint16_t window[5];
median_filter_t filter;
median_filter_init(&filter, window, 5);
uint16_t clean = median_filter_step(&filter, adc_read());
```

## When not to use

- A fast control loop with a large window; the delay and O(N^2) cost are too high.
- A stream with no impulses; `moving-average` is cheaper.

## Verification

`make test` covers odd/even windows, wrap, reset, size 1, and size 0.
Behavior reference: a production target `pir.c`; Padauk `Soft_Filter/AD_filter_02`.
