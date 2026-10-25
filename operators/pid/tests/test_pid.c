#include <stdio.h>
#include "pid.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    pid_t pid;

    pid_init(&pid, 1, 0, 0, 0, -1000, 1000, -1000, 1000);
    CHECK(pid_step(&pid, 100, 0) == 100);
    CHECK(pid_step(&pid, 100, 100) == 0);
    CHECK(pid_step(&pid, 100, 150) == -50);

    pid_init(&pid, 0, 1, 0, 0, -25, 25, -1000, 1000);
    CHECK(pid_step(&pid, 100, 0) == 25);
    CHECK(pid_step(&pid, 100, 0) == 25);
    CHECK(pid.integral == 25);

    pid_init(&pid, 0, 0, 10, 0, -1000, 1000, -1000, 1000);
    CHECK(pid_step(&pid, 10, 0) == 100);
    CHECK(pid_step(&pid, 10, 0) == 0);

    pid_init(&pid, 1, 0, 0, 1, -1000, 1000, -1000, 1000);
    CHECK(pid_step(&pid, 10, 0) == 5);

    pid_init(&pid, 100, 0, 0, 0, -1000, 1000, -10, 10);
    CHECK(pid_step(&pid, 100, 0) == 10);
    CHECK(pid_step(&pid, -100, 0) == -10);

    pid_reset(&pid);
    CHECK(pid.integral == 0 && pid.previous_error == 0);

    if (failures) return 1;
    puts("pid: ok");
    return 0;
}
