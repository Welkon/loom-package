#include "key_fsm.h"

void key_fsm_init(key_fsm_t *fsm, uint16_t debounce_ms, uint16_t long_ms)
{
    fsm->debounce_ms = debounce_ms;
    fsm->long_ms = long_ms;
    fsm->stable_ms = 0;
    fsm->held_ms = 0;
    fsm->raw_last = 0;
    fsm->pressed = 0;
    fsm->long_sent = 0;
}

key_event_t key_fsm_step(key_fsm_t *fsm, uint8_t raw, uint16_t elapsed_ms)
{
    raw = raw ? 1U : 0U;
    if (raw != fsm->raw_last) {
        fsm->raw_last = raw;
        fsm->stable_ms = 0;
    }
    if (fsm->stable_ms < fsm->debounce_ms) {
        fsm->stable_ms = (uint16_t)(fsm->stable_ms + elapsed_ms);
    }

    if (fsm->stable_ms >= fsm->debounce_ms) {
        if (raw && !fsm->pressed) {
            fsm->pressed = 1;
            fsm->held_ms = 0;
            fsm->long_sent = 0;
        } else if (!raw && fsm->pressed) {
            fsm->pressed = 0;
            if (!fsm->long_sent) {
                return KEY_EVENT_SHORT;
            }
        }
    }

    if (fsm->pressed && !fsm->long_sent) {
        fsm->held_ms = (uint16_t)(fsm->held_ms + elapsed_ms);
        if (fsm->held_ms >= fsm->long_ms) {
            fsm->long_sent = 1;
            return KEY_EVENT_LONG;
        }
    }
    return KEY_EVENT_NONE;
}
