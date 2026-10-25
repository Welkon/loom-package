#ifndef IR_NEC_H
#define IR_NEC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IR_NEC_INVALID = 0,
    IR_NEC_FRAME = 1,
    IR_NEC_REPEAT = 2
} ir_nec_result_t;

typedef struct {
    uint8_t address;
    uint8_t command;
} ir_nec_frame_t;

ir_nec_result_t ir_nec_decode(const uint16_t *intervals_us, uint8_t count, ir_nec_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif
