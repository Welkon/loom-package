#include <stdio.h>
#include "ir_capture.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    ir_capture_t capture;
    uint16_t intervals[IR_CAPTURE_INTERVALS];
    uint32_t now = 1000;
    uint8_t index;

    ir_capture_init(&capture);
    ir_capture_edge(&capture, now);
    now += 13500;
    ir_capture_edge(&capture, now);
    for (index = 0; index < 32U; index++) {
        now += (index & 1U) ? 2250U : 1120U;
        ir_capture_edge(&capture, now);
    }
    CHECK(ir_capture_ready(&capture) == 1);
    CHECK(ir_capture_take(&capture, intervals, IR_CAPTURE_INTERVALS) == IR_CAPTURE_INTERVALS);
    CHECK(intervals[0] == 13500);
    CHECK(intervals[1] == 1120);
    CHECK(intervals[2] == 2250);
    CHECK(ir_capture_ready(&capture) == 0);

    ir_capture_edge(&capture, now + 40000);
    now += 50000;
    ir_capture_edge(&capture, now);
    now += 13500;
    ir_capture_edge(&capture, now);
    CHECK(ir_capture_ready(&capture) == 0);
    CHECK(ir_capture_take(&capture, intervals, IR_CAPTURE_INTERVALS) == 0);

    if (failures) return 1;
    puts("ir_capture: ok");
    return 0;
}
