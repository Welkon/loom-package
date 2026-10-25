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
	$(BUILD)/test_ring_buffer

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

test: $(TESTS)
	@for t in $(TESTS); do echo "== $$t"; ./$$t; done

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
