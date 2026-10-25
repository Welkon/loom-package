#include <stdio.h>
#include "ir_nec.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

static void build(uint16_t *intervals, uint8_t address, uint8_t command)
{
    uint32_t data = ((uint32_t)address << 24)
        | ((uint32_t)(uint8_t)~address << 16)
        | ((uint32_t)command << 8)
        | (uint8_t)~command;
    uint8_t bit;
    intervals[0] = 13500;
    for (bit = 0; bit < 32U; bit++) {
        intervals[bit + 1U] = ((data >> (31U - bit)) & 1U) ? 2250U : 1120U;
    }
}

int main(void)
{
    uint16_t intervals[33];
    ir_nec_frame_t frame;

    build(intervals, 0x00, 0x45);
    CHECK(ir_nec_decode(intervals, 33, &frame) == IR_NEC_FRAME);
    CHECK(frame.address == 0x00 && frame.command == 0x45);

    intervals[1] = 5000;
    CHECK(ir_nec_decode(intervals, 33, &frame) == IR_NEC_INVALID);

    build(intervals, 0x12, 0x34);
    intervals[17] ^= 1U;
    intervals[17] = intervals[17] == 2250U ? 1120U : 2250U;
    CHECK(ir_nec_decode(intervals, 33, &frame) == IR_NEC_INVALID);

    intervals[0] = 11250;
    CHECK(ir_nec_decode(intervals, 1, &frame) == IR_NEC_REPEAT);
    CHECK(ir_nec_decode(intervals, 2, &frame) == IR_NEC_INVALID);

    if (failures) return 1;
    puts("ir_nec: ok");
    return 0;
}
