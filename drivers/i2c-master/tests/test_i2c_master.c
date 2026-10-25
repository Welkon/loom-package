#include <stdio.h>
#include <string.h>
#include "i2c_master.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t log[32];
    uint8_t count;
    uint8_t nack_at;
    uint8_t writes;
    uint8_t read_value;
    uint32_t now_us;
    uint32_t step_us;
} fake_t;

static void push(fake_t *fake, uint8_t byte)
{
    if (fake->count < sizeof(fake->log)) {
        fake->log[fake->count++] = byte;
    }
}

static void fake_start(void *context)
{
    push((fake_t *)context, 'S');
}

static void fake_stop(void *context)
{
    push((fake_t *)context, 'P');
}

static uint8_t fake_write(void *context, uint8_t byte)
{
    fake_t *fake = (fake_t *)context;
    fake->writes++;
    push(fake, byte);
    fake->now_us += fake->step_us;
    return fake->nack_at != 0U && fake->writes == fake->nack_at ? 0U : 1U;
}

static uint8_t fake_read(void *context, uint8_t ack)
{
    fake_t *fake = (fake_t *)context;
    push(fake, ack ? 'A' : 'N');
    fake->now_us += fake->step_us;
    return fake->read_value++;
}

static uint32_t fake_now(void *context)
{
    return ((fake_t *)context)->now_us;
}

static i2c_port_t fake_port(fake_t *fake)
{
    i2c_port_t port;
    port.context = fake;
    port.start = fake_start;
    port.stop = fake_stop;
    port.write = fake_write;
    port.read = fake_read;
    port.now_us = fake_now;
    return port;
}

static int log_is(const fake_t *fake, const uint8_t *expected, uint8_t length)
{
    return fake->count == length && memcmp(fake->log, expected, length) == 0;
}

int main(void)
{
    fake_t fake;
    i2c_master_t master;
    uint8_t data[2] = {1, 2};
    uint8_t out[2] = {0, 0};
    const uint8_t write_log[] = {'S', 0xA0, 1, 2, 'P'};
    const uint8_t nack_log[] = {'S', 0xA0, 'P'};
    const uint8_t read_log[] = {'S', 0xA1, 'A', 'N', 'P'};
    const uint8_t write_read_log[] = {'S', 0xA0, 0x10, 'S', 0xA1, 'N', 'P'};
    const uint8_t timeout_log[] = {'S', 0xA0, 'P'};

    memset(&fake, 0, sizeof(fake));
    i2c_master_init(&master, fake_port(&fake), 0);
    CHECK(i2c_master_write(&master, 0x50, data, 2) == I2C_OK);
    CHECK(log_is(&fake, write_log, sizeof(write_log)));

    memset(&fake, 0, sizeof(fake));
    fake.nack_at = 1;
    i2c_master_init(&master, fake_port(&fake), 0);
    CHECK(i2c_master_write(&master, 0x50, data, 2) == I2C_NACK);
    CHECK(log_is(&fake, nack_log, sizeof(nack_log)));

    memset(&fake, 0, sizeof(fake));
    fake.read_value = 0x10;
    i2c_master_init(&master, fake_port(&fake), 0);
    CHECK(i2c_master_read(&master, 0x50, out, 2) == I2C_OK);
    CHECK(out[0] == 0x10 && out[1] == 0x11);
    CHECK(log_is(&fake, read_log, sizeof(read_log)));

    memset(&fake, 0, sizeof(fake));
    fake.read_value = 0x20;
    i2c_master_init(&master, fake_port(&fake), 0);
    {
        uint8_t reg = 0x10;
        CHECK(i2c_master_write_read(&master, 0x50, &reg, 1, out, 1) == I2C_OK);
    }
    CHECK(out[0] == 0x20);
    CHECK(log_is(&fake, write_read_log, sizeof(write_read_log)));

    memset(&fake, 0, sizeof(fake));
    fake.step_us = 100;
    i2c_master_init(&master, fake_port(&fake), 50);
    CHECK(i2c_master_write(&master, 0x50, data, 2) == I2C_TIMEOUT);
    CHECK(log_is(&fake, timeout_log, sizeof(timeout_log)));

    if (failures) return 1;
    puts("i2c_master: ok");
    return 0;
}
