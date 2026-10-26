# emb-loom

Catalog website: <https://loom.pcbsmt.me>

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

The catalog currently contains fifty-two units.

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
- `operators/pid` — integer PID controller.
- `operators/gamma-lut` — interpolated lookup table.
- `operators/crc32` — CRC-32/ISO-HDLC.
- `operators/fixed-sqrt` — Q16.16 square root.
- `operators/biquad` — Q15 direct-form I biquad.
- `operators/fir` — Q15 finite-impulse-response filter.

Drivers:

- `drivers/i2c-master` — blocking I2C master core with a port and fake bus.
- `drivers/uart` — blocking UART core with a port and timeout.
- `drivers/ws2812` — RGB to GRB encoder with a timing port.
- `drivers/ir-capture` — falling-edge interval collector for IR receivers.
- `drivers/adc-sampler` — averaged ADC reads.
- `drivers/soft-pwm` — software PWM on one pin.
- `drivers/ds18b20` — DS18B20 temperature over 1-Wire.
- `drivers/ssd1306` — SSD1306 128x64 I2C display.
- `drivers/dht` — DHT11/DHT22 frame decoders.
- `drivers/rotary-encoder` — Quadrature decoder.
- `drivers/at24cxx` — AT24Cxx I2C EEPROM.
- `drivers/ds3231` — DS3231 I2C real-time clock.
- `drivers/max7219` — MAX7219/MAX7221 SPI LED driver.
- `drivers/pca9685` — PCA9685 16-channel I2C PWM.
- `drivers/mpu6050` — MPU-6050 six-axis IMU.
- `drivers/sht3x` — SHT30/SHT31/SHT35 temperature and humidity.
- `drivers/aht20` — AHT20 temperature and humidity.
- `drivers/mcp2515` — MCP2515 CAN controller.
- `drivers/stepper` — A4988/DRV8825 step/direction driver.
- `drivers/hx711` — HX711 24-bit load-cell amplifier.
- `drivers/max31855` — MAX31855 thermocouple amplifier.
- `drivers/bme280` — BME280 temperature, pressure, and humidity.
- `drivers/adxl345` — ADXL345 three-axis accelerometer.
- `drivers/mcp23017` — MCP23017 16-bit I/O expander.
- `drivers/ads1115` — ADS1115 16-bit ADC.
- `drivers/sh1106` — SH1106 128x64 I2C OLED.
- `drivers/mcp4725` — MCP4725 12-bit DAC.
- `drivers/pcf8574` — PCF8574 8-bit I/O expander.
- `drivers/hc-sr04` — HC-SR04 ultrasonic distance.
- `drivers/ina219` — INA219 current/power monitor.
- `drivers/mcp9808` — MCP9808 temperature sensor.

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
