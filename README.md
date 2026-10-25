# emb-loom

Reusable embedded drivers and algorithms. `loom` is a supply of tested
building blocks: weave them into firmware, and keep product logic in the
product repository.

## Layout

```
operators/   pure algorithms, no I/O
drivers/     protocol/state-machine cores with a port.h
adapters/    MCU-specific register implementations (not portable)
patterns/    structure guides, not code
loom.json    unit index
```

The catalog currently contains nineteen units.

Operators:

- `operators/filters/clamp` — limit sample-to-sample change.
- `operators/filters/median` — remove impulses with an N-sample median.
- `operators/filters/moving-average` — smooth with an N-sample mean.
- `operators/filters/ema` — first-order lag with a power-of-two shift.
- `operators/filters/trimmed-mean` — drop min/max, then average a batch.
- `operators/filters/mean` — arithmetic mean of a batch.
- `operators/filters/clamp-average` — clamp, then moving average.
- `operators/filters/weighted-average` — weighted mean of a batch.
- `operators/filters/clamp-debounce` — clamp, then N-sample debounce.
- `operators/debounce` — stabilize a binary input.
- `operators/hysteresis` — keep state across a threshold band.
- `operators/key-fsm` — short and long key events.
- `operators/crc16` — CRC-16/CCITT-FALSE.
- `operators/ring-buffer` — fixed-size byte ring buffer.
- `operators/ir-nec-decode` — NEC IR frame decoder.

Drivers:

- `drivers/i2c-master` — blocking I2C master core with a port and fake bus.
- `drivers/uart` — blocking UART core with a port and timeout.
- `drivers/ws2812` — RGB to GRB encoder with a timing port.
- `drivers/ir-capture` — falling-edge interval collector for IR receivers.

## Contract

- C99, no malloc, no vendor headers, no I/O in operators.
- The caller owns storage; no hidden globals.
- Each unit carries host tests and a short contract in its README.
- MIT licensed.

## Test

```sh
make test
```

## Use

Copy the selected unit into the product tree, keep its provenance, and call
the public API. Adapters for a specific MCU stay next to the product.

## Versioning

`loom.json` lists unit versions. A project pins exact versions and file
hashes in its own lock file.
