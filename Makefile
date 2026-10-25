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
	$(BUILD)/test_ir_capture

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

test: $(TESTS)
	@for t in $(TESTS); do echo "== $$t"; ./$$t; done

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
