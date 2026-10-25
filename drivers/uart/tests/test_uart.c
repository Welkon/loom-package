#include <stdio.h>
#include <string.h>
#include "uart.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t written[16];
    uint8_t count;
    uint8_t ready_after;
    uint8_t ready_calls;
    uint8_t read_value;
    uint32_t now_us;
    uint32_t step_us;
} fake_t;

static void fake_write(void *context, uint8_t byte)
{
    fake_t *fake = (fake_t *)context;
    if (fake->count < sizeof(fake->written)) fake->written[fake->count++] = byte;
}

static uint8_t fake_ready(void *context)
{
    fake_t *fake = (fake_t *)context;
    fake->ready_calls++;
    fake->now_us += fake->step_us;
    return fake->ready_calls >= fake->ready_after ? 1U : 0U;
}

static uint8_t fake_read(void *context)
{
    fake_t *fake = (fake_t *)context;
    return fake->read_value++;
}

static uint32_t fake_now(void *context)
{
    return ((fake_t *)context)->now_us;
}

static uart_port_t fake_port(fake_t *fake)
{
    uart_port_t port;
    port.context = fake;
    port.write = fake_write;
    port.read_ready = fake_ready;
    port.read = fake_read;
    port.now_us = fake_now;
    return port;
}

int main(void)
{
    fake_t fake;
    uart_t link;
    uint8_t reply[2];
    const uint8_t hello[3] = {'A', 'T', '\n'};

    memset(&fake, 0, sizeof(fake));
    fake.ready_after = 1;
    uart_init(&link, fake_port(&fake), 0);
    uart_write(&link, hello, 3);
    CHECK(fake.count == 3 && memcmp(fake.written, hello, 3) == 0);

    memset(&fake, 0, sizeof(fake));
    fake.ready_after = 1;
    fake.read_value = 0x10;
    uart_init(&link, fake_port(&fake), 0);
    CHECK(uart_read(&link, reply, 2) == UART_OK);
    CHECK(reply[0] == 0x10 && reply[1] == 0x11);

    memset(&fake, 0, sizeof(fake));
    fake.ready_after = 100;
    fake.step_us = 10;
    uart_init(&link, fake_port(&fake), 50);
    CHECK(uart_read(&link, reply, 1) == UART_TIMEOUT);

    if (failures) return 1;
    puts("uart: ok");
    return 0;
}
