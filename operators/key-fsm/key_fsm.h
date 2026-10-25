#ifndef KEY_FSM_H
#define KEY_FSM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT = 1,
    KEY_EVENT_LONG = 2
} key_event_t;

typedef struct {
    uint16_t debounce_ms;
    uint16_t long_ms;
    uint16_t stable_ms;
    uint16_t held_ms;
    uint8_t raw_last;
    uint8_t pressed;
    uint8_t long_sent;
} key_fsm_t;

void key_fsm_init(key_fsm_t *fsm, uint16_t debounce_ms, uint16_t long_ms);
key_event_t key_fsm_step(key_fsm_t *fsm, uint8_t raw, uint16_t elapsed_ms);

#ifdef __cplusplus
}
#endif

#endif
