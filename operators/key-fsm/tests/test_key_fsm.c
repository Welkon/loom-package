#include <stdio.h>
#include "key_fsm.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    key_fsm_t fsm;

    key_fsm_init(&fsm, 20, 1000);
    CHECK(key_fsm_step(&fsm, 1, 20) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 1, 500) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 1, 500) == KEY_EVENT_LONG);
    CHECK(key_fsm_step(&fsm, 0, 20) == KEY_EVENT_NONE);

    key_fsm_init(&fsm, 20, 1000);
    CHECK(key_fsm_step(&fsm, 1, 20) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 1, 100) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 0, 20) == KEY_EVENT_SHORT);

    key_fsm_init(&fsm, 20, 1000);
    CHECK(key_fsm_step(&fsm, 1, 10) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 0, 10) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 1, 20) == KEY_EVENT_NONE);
    CHECK(key_fsm_step(&fsm, 0, 20) == KEY_EVENT_SHORT);

    key_fsm_init(&fsm, 0, 0);
    CHECK(key_fsm_step(&fsm, 1, 0) == KEY_EVENT_LONG);
    CHECK(key_fsm_step(&fsm, 0, 0) == KEY_EVENT_NONE);

    if (failures) return 1;
    puts("key_fsm: ok");
    return 0;
}
