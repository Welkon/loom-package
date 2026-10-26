# trimmed-mean

Average a batch after dropping one minimum and one maximum.

## Application scenarios

- PIR acquisition uses a 6-sample batch; one bad sample must not move the result.
- An ADC burst contains a spike from a switching load.
- A sensor batch has one stuck or saturated reading.

## Contract

```c
uint16_t trimmed_mean(const uint16_t *samples, uint8_t count);
```

- `count` must be 3..255. Smaller batches return 0.
- One minimum and one maximum are removed; duplicates are handled as values.
- Result is rounded to the nearest integer.
- No allocation; O(N).

## Parameters

- Batch size: 5..9 is typical. Larger batches reject more impulses but add latency.
- Duplicate min/max reduce the effective trim; document the expected distribution.

## Memory and timing

- No state; the caller owns the batch.
- O(N) with one pass for sum/min/max.

## Integration

```c
uint16_t batch[6];
/* fill batch from the sensor, then: */
uint16_t value = trimmed_mean(batch, 6);
```

## When not to use

- A continuous stream without batching; use `median` or `moving-average`.
- The batch may contain more than one bad sample; a larger trim or median is safer.

## Verification

`make test` covers the 6-sample PIR batch, 3-sample minimum, duplicates, and invalid counts.
Behavior reference: a production trimmed-mean filter; this implementation is original.
