# pir

PIR motion detector signal chain for sampled pyroelectric data.

## Application scenarios

- A raw PIR/DOCI front end that outputs digitized samples.
- A battery motion sensor with warm-up and output lock.
- A product that needs a 6-sample trimmed-mean pyroelectric chain.

## Contract

```c
void pir_init(pir_t *pir, uint16_t motion_span);
uint8_t pir_step(pir_t *pir, uint16_t sample, uint32_t now_ms);
void pir_set_warm(pir_t *pir, uint8_t warm);
void pir_output_changed(pir_t *pir, uint32_t now_ms);
```

- `step` consumes one sample and returns 1 when sustained motion is published.
- Warm-up, jam, and output locks inhibit events but not acquisition/filtering.
- `motion_span` is the peak-to-peak window threshold in sample units.
- `output_changed` restarts the 3 s output lock; two consecutive jumps over
  the jam delta start the 5 s error lock.

## Parameters

- 6 samples per batch; drop min/max, average the middle four.
- 6 batch averages form the window; 16 warm-up batches are skipped.
- Three strict same-direction pairs at window positions (5,4), (3,2), (1,0)
  publish an event.
- Defaults: jam delta 400, output lock 3000 ms, error lock 5000 ms.
  Tune `motion_span` for the selected sensitivity.

## Memory and timing

- 32 bytes of state; no allocation.
- One bounded 6-entry scan per batch; O(1) per sample.
- Call `step` at the sensor's sample cadence; do not replay old samples.

## Integration

```c
pir_t pir;
pir_init(&pir, 245);
pir_set_warm(&pir, 1);
for (;;) {
    uint16_t sample = read_pir_sample();
    if (pir_step(&pir, sample, millis())) {
        publish_motion();
    }
}
```

## Adapters

- The adapter powers the module, provides the sample frame, and restarts the
  output lock when the product output changes.
- Behavior reference: a 6-sample trimmed-mean pyroelectric chain with
  three-pair slope detection; this implementation is original.

## When not to use

- A digital HC-SR501 output; use a debounce/hold timer instead.
- A sensor without a controlled sample cadence or timestamp.

## Verification

`make test` covers the warm-up skip, a rising window event, a falling window,
the span threshold, and the double-jump error lock.
