CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic -O2
BUILD := build

TESTS := \
	$(BUILD)/test_clamp_filter \
	$(BUILD)/test_median_filter \
	$(BUILD)/test_moving_average \
	$(BUILD)/test_ema_filter \
	$(BUILD)/test_trimmed_mean \
	$(BUILD)/test_debounce \
	$(BUILD)/test_hysteresis \
	$(BUILD)/test_key_fsm \
	$(BUILD)/test_crc16 \
	$(BUILD)/test_ring_buffer \
	$(BUILD)/test_mean \
	$(BUILD)/test_clamp_average \
	$(BUILD)/test_weighted_average \
	$(BUILD)/test_clamp_debounce \
	$(BUILD)/test_ir_nec \
	$(BUILD)/test_i2c_master \
	$(BUILD)/test_uart \
	$(BUILD)/test_ws2812 \
	$(BUILD)/test_ir_capture \
	$(BUILD)/test_pid \
	$(BUILD)/test_gamma_lut \
	$(BUILD)/test_adc_sampler \
	$(BUILD)/test_soft_pwm \
	$(BUILD)/test_crc32 \
	$(BUILD)/test_fixed_sqrt \
	$(BUILD)/test_ds18b20 \
	$(BUILD)/test_ssd1306 \
	$(BUILD)/test_biquad \
	$(BUILD)/test_dht \
	$(BUILD)/test_rotary_encoder \
	$(BUILD)/test_fir \
	$(BUILD)/test_at24cxx \
	$(BUILD)/test_ds3231 \
	$(BUILD)/test_max7219 \
	$(BUILD)/test_pca9685 \
	$(BUILD)/test_mpu6050 \
	$(BUILD)/test_sht3x \
	$(BUILD)/test_aht20 \
	$(BUILD)/test_mcp2515 \
	$(BUILD)/test_stepper \
	$(BUILD)/test_hx711 \
	$(BUILD)/test_max31855 \
	$(BUILD)/test_adxl345 \
	$(BUILD)/test_bme280 \
	$(BUILD)/test_mcp23017 \
	$(BUILD)/test_ads1115 \
	$(BUILD)/test_sh1106 \
	$(BUILD)/test_mcp4725 \
	$(BUILD)/test_pcf8574 \
	$(BUILD)/test_hc_sr04 \
	$(BUILD)/test_ina219 \
	$(BUILD)/test_mcp9808 \
	$(BUILD)/test_nrf24l01 \
	$(BUILD)/test_hd44780 \
	$(BUILD)/test_mfrc522 \
	$(BUILD)/test_modbus_rtu \
	$(BUILD)/test_pir \
	$(BUILD)/test_scd4x \
	$(BUILD)/test_sdcard

all: test

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/test_clamp_filter: operators/filters/clamp/tests/test_clamp_filter.c operators/filters/clamp/clamp_filter.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/clamp $^ -o $@

$(BUILD)/test_median_filter: operators/filters/median/tests/test_median_filter.c operators/filters/median/median_filter.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/median $^ -o $@

$(BUILD)/test_moving_average: operators/filters/moving-average/tests/test_moving_average.c operators/filters/moving-average/moving_average.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/moving-average $^ -o $@

$(BUILD)/test_ema_filter: operators/filters/ema/tests/test_ema_filter.c operators/filters/ema/ema_filter.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/ema $^ -o $@

$(BUILD)/test_trimmed_mean: operators/filters/trimmed-mean/tests/test_trimmed_mean.c operators/filters/trimmed-mean/trimmed_mean.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/trimmed-mean $^ -o $@

$(BUILD)/test_debounce: operators/debounce/tests/test_debounce.c operators/debounce/debounce.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/debounce $^ -o $@

$(BUILD)/test_hysteresis: operators/hysteresis/tests/test_hysteresis.c operators/hysteresis/hysteresis.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/hysteresis $^ -o $@

$(BUILD)/test_key_fsm: operators/key-fsm/tests/test_key_fsm.c operators/key-fsm/key_fsm.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/key-fsm $^ -o $@

$(BUILD)/test_crc16: operators/crc16/tests/test_crc16.c operators/crc16/crc16.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/crc16 $^ -o $@

$(BUILD)/test_ring_buffer: operators/ring-buffer/tests/test_ring_buffer.c operators/ring-buffer/ring_buffer.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/ring-buffer $^ -o $@

$(BUILD)/test_mean: operators/filters/mean/tests/test_mean.c operators/filters/mean/mean.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/mean $^ -o $@

$(BUILD)/test_clamp_average: operators/filters/clamp-average/tests/test_clamp_average.c operators/filters/clamp-average/clamp_average.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/clamp-average $^ -o $@

$(BUILD)/test_weighted_average: operators/filters/weighted-average/tests/test_weighted_average.c operators/filters/weighted-average/weighted_average.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/weighted-average $^ -o $@

$(BUILD)/test_clamp_debounce: operators/filters/clamp-debounce/tests/test_clamp_debounce.c operators/filters/clamp-debounce/clamp_debounce.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/filters/clamp-debounce $^ -o $@

$(BUILD)/test_ir_nec: operators/ir-nec-decode/tests/test_ir_nec.c operators/ir-nec-decode/ir_nec.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/ir-nec-decode $^ -o $@

$(BUILD)/test_i2c_master: drivers/i2c-master/tests/test_i2c_master.c drivers/i2c-master/i2c_master.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/i2c-master $^ -o $@

$(BUILD)/test_uart: drivers/uart/tests/test_uart.c drivers/uart/uart.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/uart $^ -o $@

$(BUILD)/test_ws2812: drivers/ws2812/tests/test_ws2812.c drivers/ws2812/ws2812.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ws2812 $^ -o $@

$(BUILD)/test_ir_capture: drivers/ir-capture/tests/test_ir_capture.c drivers/ir-capture/ir_capture.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ir-capture $^ -o $@

$(BUILD)/test_pid: operators/pid/tests/test_pid.c operators/pid/pid.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/pid $^ -o $@

$(BUILD)/test_gamma_lut: operators/gamma-lut/tests/test_gamma_lut.c operators/gamma-lut/gamma_lut.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/gamma-lut $^ -o $@

$(BUILD)/test_adc_sampler: drivers/adc-sampler/tests/test_adc_sampler.c drivers/adc-sampler/adc_sampler.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/adc-sampler $^ -o $@

$(BUILD)/test_soft_pwm: drivers/soft-pwm/tests/test_soft_pwm.c drivers/soft-pwm/soft_pwm.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/soft-pwm $^ -o $@

$(BUILD)/test_crc32: operators/crc32/tests/test_crc32.c operators/crc32/crc32.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/crc32 $^ -o $@

$(BUILD)/test_fixed_sqrt: operators/fixed-sqrt/tests/test_fixed_sqrt.c operators/fixed-sqrt/fixed_sqrt.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/fixed-sqrt $^ -o $@

$(BUILD)/test_ds18b20: drivers/ds18b20/tests/test_ds18b20.c drivers/ds18b20/ds18b20.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ds18b20 $^ -o $@

$(BUILD)/test_ssd1306: drivers/ssd1306/tests/test_ssd1306.c drivers/ssd1306/ssd1306.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ssd1306 $^ -o $@

$(BUILD)/test_biquad: operators/biquad/tests/test_biquad.c operators/biquad/biquad.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/biquad $^ -o $@

$(BUILD)/test_dht: drivers/dht/tests/test_dht.c drivers/dht/dht.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/dht $^ -o $@

$(BUILD)/test_rotary_encoder: drivers/rotary-encoder/tests/test_rotary_encoder.c drivers/rotary-encoder/rotary_encoder.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/rotary-encoder $^ -o $@

$(BUILD)/test_fir: operators/fir/tests/test_fir.c operators/fir/fir.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/fir $^ -o $@

$(BUILD)/test_at24cxx: drivers/at24cxx/tests/test_at24cxx.c drivers/at24cxx/at24cxx.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/at24cxx $^ -o $@

$(BUILD)/test_ds3231: drivers/ds3231/tests/test_ds3231.c drivers/ds3231/ds3231.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ds3231 $^ -o $@

$(BUILD)/test_max7219: drivers/max7219/tests/test_max7219.c drivers/max7219/max7219.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/max7219 $^ -o $@

$(BUILD)/test_pca9685: drivers/pca9685/tests/test_pca9685.c drivers/pca9685/pca9685.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/pca9685 $^ -o $@

$(BUILD)/test_mpu6050: drivers/mpu6050/tests/test_mpu6050.c drivers/mpu6050/mpu6050.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/mpu6050 $^ -o $@

$(BUILD)/test_sht3x: drivers/sht3x/tests/test_sht3x.c drivers/sht3x/sht3x.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/sht3x $^ -o $@

$(BUILD)/test_aht20: drivers/aht20/tests/test_aht20.c drivers/aht20/aht20.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/aht20 $^ -o $@

$(BUILD)/test_mcp2515: drivers/mcp2515/tests/test_mcp2515.c drivers/mcp2515/mcp2515.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/mcp2515 $^ -o $@

$(BUILD)/test_stepper: drivers/stepper/tests/test_stepper.c drivers/stepper/stepper.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/stepper $^ -o $@

$(BUILD)/test_hx711: drivers/hx711/tests/test_hx711.c drivers/hx711/hx711.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/hx711 $^ -o $@

$(BUILD)/test_max31855: drivers/max31855/tests/test_max31855.c drivers/max31855/max31855.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/max31855 $^ -o $@

$(BUILD)/test_adxl345: drivers/adxl345/tests/test_adxl345.c drivers/adxl345/adxl345.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/adxl345 $^ -o $@

$(BUILD)/test_bme280: drivers/bme280/tests/test_bme280.c drivers/bme280/bme280.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/bme280 $^ -o $@

$(BUILD)/test_mcp23017: drivers/mcp23017/tests/test_mcp23017.c drivers/mcp23017/mcp23017.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/mcp23017 $^ -o $@

$(BUILD)/test_ads1115: drivers/ads1115/tests/test_ads1115.c drivers/ads1115/ads1115.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ads1115 $^ -o $@

$(BUILD)/test_sh1106: drivers/sh1106/tests/test_sh1106.c drivers/sh1106/sh1106.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/sh1106 $^ -o $@

$(BUILD)/test_mcp4725: drivers/mcp4725/tests/test_mcp4725.c drivers/mcp4725/mcp4725.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/mcp4725 $^ -o $@

$(BUILD)/test_pcf8574: drivers/pcf8574/tests/test_pcf8574.c drivers/pcf8574/pcf8574.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/pcf8574 $^ -o $@

$(BUILD)/test_hc_sr04: drivers/hc-sr04/tests/test_hc_sr04.c drivers/hc-sr04/hc_sr04.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/hc-sr04 $^ -o $@

$(BUILD)/test_ina219: drivers/ina219/tests/test_ina219.c drivers/ina219/ina219.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/ina219 $^ -o $@

$(BUILD)/test_mcp9808: drivers/mcp9808/tests/test_mcp9808.c drivers/mcp9808/mcp9808.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/mcp9808 $^ -o $@

$(BUILD)/test_nrf24l01: drivers/nrf24l01/tests/test_nrf24l01.c drivers/nrf24l01/nrf24l01.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/nrf24l01 $^ -o $@

$(BUILD)/test_hd44780: drivers/hd44780/tests/test_hd44780.c drivers/hd44780/hd44780.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/hd44780 $^ -o $@

$(BUILD)/test_mfrc522: drivers/mfrc522/tests/test_mfrc522.c drivers/mfrc522/mfrc522.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/mfrc522 $^ -o $@

$(BUILD)/test_modbus_rtu: operators/modbus-rtu/tests/test_modbus_rtu.c operators/modbus-rtu/modbus_rtu.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/modbus-rtu $^ -o $@

$(BUILD)/test_pir: operators/pir/tests/test_pir.c operators/pir/pir.c | $(BUILD)
	$(CC) $(CFLAGS) -Ioperators/pir $^ -o $@

$(BUILD)/test_scd4x: drivers/scd4x/tests/test_scd4x.c drivers/scd4x/scd4x.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/scd4x $^ -o $@

$(BUILD)/test_sdcard: drivers/sdcard/tests/test_sdcard.c drivers/sdcard/sdcard.c | $(BUILD)
	$(CC) $(CFLAGS) -Idrivers/sdcard $^ -o $@

test: $(TESTS)
	@for t in $(TESTS); do echo "== $$t"; ./$$t; done

clean:
	rm -rf $(BUILD)

.PHONY: all test clean site deploy-site check-catalog

site:
	node site/build.mjs

deploy-site:
	./site/deploy.sh

check-catalog:
	node tools/loom/check-catalog.mjs
