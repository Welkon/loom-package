# Changelog

## 0.4.0

- Add the `uart` driver with a port and timeout.
- Add the `ws2812` RGB/GRB driver with a timing port.
- Add the `ir-capture` falling-edge interval collector.

## 0.3.1

- Expand every unit README with application scenarios, parameters, memory,
  timing, integration code, and when not to use.
- Add `applications` metadata to `loom.json`.

## 0.3.0

- Add `mean`, `clamp-average`, `weighted-average`, and `clamp-debounce`
  from the Padauk Soft_Filter family.
- Add `ir-nec-decode` from a production target and Padauk IR examples.
- Add the `i2c-master` driver core with a port and fake bus.

## 0.2.0

- Add `trimmed-mean`, `debounce`, `hysteresis`, `key-fsm`, `crc16`, and
  `ring-buffer`.
- Add host tests for every unit.

## 0.1.0

- Add `clamp`, `median`, `moving-average`, and `ema`.
