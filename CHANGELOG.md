# Changelog

## Unreleased

- Move hosting and CI to GitHub (`Welkon/loom`); source, raw, blob, and issue
  links follow the new remote.
- Publish a Cargo-style static registry: `index.json` plus per-unit tarballs,
  so `loom add <id> --index https://loom.pcbsmt.me` downloads one unit.
- Yank a unit version without deleting history: the version is recorded in
  `loom.json`, stays in the index and tarball history, and `loom add` refuses
  an exact yanked version.
- Check the published pin, not only the working tree: `lock.mjs --check`
  rejects a unit whose content at the lock ref disagrees with its locked
  hash, and refuses a lock whose ref does not exist.
- Enforce the README contract in `check-catalog.mjs`: purpose paragraph,
  sections, and `Adapters` for drivers.

## 0.23.0

- Add `loom.lock`: a pinned catalog commit and a SHA-256 per unit.
- Add immutable `unit/<id>/v<version>` tags, with `tag-units.mjs` to create
  and check them.
- Release `median` 0.2.0.

## 0.22.0

- Add `sdcard` from the SD SPI specification and an Apache-2.0 reference.

## 0.21.0

- Add `scd4x` from LibDriver (MIT).

## 0.20.0

- Add `pir`, a 6-sample trimmed-mean pyroelectric motion chain.
- Anonymize provenance references; remove internal project identifiers.

## 0.19.0

- Add `modbus-rtu` from the Modbus serial-line specification.

## 0.18.0

- Add `mfrc522` from LibDriver (MIT).

## 0.17.0

- Add `hd44780` from the HD44780 datasheet and the MIT 4-bit sequence.

## 0.16.0

- Add `nrf24l01` from LibDriver (MIT).

## 0.15.0

- Add `ina219` from LibDriver (MIT).
- Add `mcp9808` from LibDriver (MIT).

## 0.14.0

- Add `pcf8574` from LibDriver (MIT).
- Add `hc-sr04` from LibDriver (MIT).

## 0.13.0

- Add `sh1106` from the u8g2 SH1106 sequence (BSD-2-Clause).
- Add `mcp4725` from LibDriver (MIT).

## 0.12.0

- Add `mcp23017` from blemasle/arduino-mcp23017 (MIT).
- Add `ads1115` from LibDriver (MIT).

## 0.11.0

- Add `bme280` using the Bosch integer compensation formulas (BSD-3-Clause).
- Add `adxl345` from LibDriver (MIT).

## 0.10.0

- Add `stepper` from laurb9/StepperDriver (MIT).
- Add `hx711` from bogde/HX711 (MIT).
- Add `max31855` from LibDriver (MIT).

## 0.9.0

- Add `sht3x` from DFRobot_SHT3x (MIT).
- Add `aht20` from LibDriver (MIT).
- Add `mcp2515` from autowp/arduino-mcp2515 (MIT).

## 0.8.0

- Add `at24cxx` from LibDriver (MIT).
- Add `ds3231` from Adafruit RTClib (MIT).
- Add `max7219` from wayoda/LedControl (MIT).
- Add `pca9685` from LibDriver (MIT).
- Add `mpu6050` from jrowberg/i2cdevlib and LibDriver (MIT).

## 0.7.0

- Add `dht` from the Adafruit DHT library behavior and the datasheets.
- Add `rotary-encoder` from the PaulStoffregen/Encoder state-table behavior.
- Add `fir` from the CMSIS-DSP Q15 form.

## 0.6.0

- Add `biquad` from the CMSIS-DSP Q15 form.
- Add `ssd1306` from the u8g2 128x64 init sequence.

## 0.5.0

- Add `pid`, `gamma-lut`, `crc32`, and `fixed-sqrt` operators.
- Add `adc-sampler`, `soft-pwm`, and `ds18b20` drivers.
- Sources and behavior references: Arduino PID (MIT), zlib (zlib license),
  libfixmath (MIT), OneWire (MIT), Padauk examples, and the DS18B20 datasheet.

## 0.4.1

- Add compatibility metadata to every unit: C99, no allocation, no float,
  and blocking behavior.
- Run the C unit tests in the release gate.

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
- Add `ir-nec-decode` from the NEC protocol and a vendor IR example.
- Add the `i2c-master` driver core with a port and fake bus.

## 0.2.0

- Add `trimmed-mean`, `debounce`, `hysteresis`, `key-fsm`, `crc16`, and
  `ring-buffer`.
- Add host tests for every unit.

## 0.1.0

- Add `clamp`, `median`, `moving-average`, and `ema`.
