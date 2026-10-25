#include "ir_capture.h"

void ir_capture_init(ir_capture_t *capture)
{
    ir_capture_reset(capture);
    capture->last_us = 0;
    capture->active = 0;
}

void ir_capture_reset(ir_capture_t *capture)
{
    capture->count = 0;
    capture->ready = 0;
    capture->active = 0;
}

void ir_capture_edge(ir_capture_t *capture, uint32_t now_us)
{
    uint32_t delta;

    if (!capture->active) {
        capture->active = 1;
        capture->last_us = now_us;
        capture->count = 0;
        capture->ready = 0;
        return;
    }
    delta = now_us - capture->last_us;
    capture->last_us = now_us;
    if (delta > IR_CAPTURE_GAP_US) {
        capture->count = 0;
        capture->ready = 0;
        return;
    }
    if (capture->count < IR_CAPTURE_INTERVALS) {
        capture->intervals[capture->count++] = (uint16_t)delta;
        if (capture->count == IR_CAPTURE_INTERVALS) {
            capture->ready = 1;
        }
    }
}

uint8_t ir_capture_ready(const ir_capture_t *capture)
{
    return capture->ready;
}

uint8_t ir_capture_take(ir_capture_t *capture, uint16_t *intervals, uint8_t max_count)
{
    uint8_t index;
    if (!capture->ready || intervals == 0 || max_count < IR_CAPTURE_INTERVALS) {
        return 0;
    }
    for (index = 0; index < IR_CAPTURE_INTERVALS; index++) {
        intervals[index] = capture->intervals[index];
    }
    ir_capture_reset(capture);
    return IR_CAPTURE_INTERVALS;
}
