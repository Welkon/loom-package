#include "ir_nec.h"

#define IR_NEC_REPEAT_MIN 10000U
#define IR_NEC_REPEAT_MAX 11999U
#define IR_NEC_LEADER_MIN 12000U
#define IR_NEC_LEADER_MAX 16000U
#define IR_NEC_BIT0_MIN 700U
#define IR_NEC_BIT0_MAX 1500U
#define IR_NEC_BIT1_MIN 1800U
#define IR_NEC_BIT1_MAX 2800U

ir_nec_result_t ir_nec_decode(const uint16_t *intervals_us, uint8_t count, ir_nec_frame_t *frame)
{
    uint32_t data = 0;
    uint8_t index;

    if (intervals_us == 0 || frame == 0) {
        return IR_NEC_INVALID;
    }
    if (count == 1U) {
        uint16_t interval = intervals_us[0];
        return (interval >= IR_NEC_REPEAT_MIN && interval <= IR_NEC_REPEAT_MAX)
            ? IR_NEC_REPEAT
            : IR_NEC_INVALID;
    }
    if (count != 33U) {
        return IR_NEC_INVALID;
    }
    if (intervals_us[0] < IR_NEC_LEADER_MIN || intervals_us[0] > IR_NEC_LEADER_MAX) {
        return IR_NEC_INVALID;
    }
    for (index = 1; index < 33U; index++) {
        uint16_t interval = intervals_us[index];
        if (interval >= IR_NEC_BIT0_MIN && interval <= IR_NEC_BIT0_MAX) {
            data = (data << 1);
        } else if (interval >= IR_NEC_BIT1_MIN && interval <= IR_NEC_BIT1_MAX) {
            data = (data << 1) | 1U;
        } else {
            return IR_NEC_INVALID;
        }
    }
    {
        uint8_t address = (uint8_t)(data >> 24);
        uint8_t address_inverse = (uint8_t)(data >> 16);
        uint8_t command = (uint8_t)(data >> 8);
        uint8_t command_inverse = (uint8_t)data;
        if ((uint8_t)(address ^ address_inverse) != 0xFFU) {
            return IR_NEC_INVALID;
        }
        if ((uint8_t)(command ^ command_inverse) != 0xFFU) {
            return IR_NEC_INVALID;
        }
        frame->address = address;
        frame->command = command;
    }
    return IR_NEC_FRAME;
}
