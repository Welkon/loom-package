#ifndef IR_CAPTURE_H
#define IR_CAPTURE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IR_CAPTURE_INTERVALS 33U
#define IR_CAPTURE_GAP_US 30000U

typedef struct {
    uint16_t intervals[IR_CAPTURE_INTERVALS];
    uint8_t count;
    uint32_t last_us;
    uint8_t active;
    uint8_t ready;
} ir_capture_t;

void ir_capture_init(ir_capture_t *capture);
void ir_capture_reset(ir_capture_t *capture);
void ir_capture_edge(ir_capture_t *capture, uint32_t now_us);
uint8_t ir_capture_ready(const ir_capture_t *capture);
uint8_t ir_capture_take(ir_capture_t *capture, uint16_t *intervals, uint8_t max_count);

#ifdef __cplusplus
}
#endif

#endif
