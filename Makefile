CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic -O2
BUILD := build

TESTS := \
	$(BUILD)/test_clamp_filter \
	$(BUILD)/test_median_filter \
	$(BUILD)/test_moving_average \
	$(BUILD)/test_ema_filter

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

test: $(TESTS)
	@for t in $(TESTS); do echo "== $$t"; ./$$t; done

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
