#include <stdio.h>
#include <string.h>
#include "at24cxx.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t memory[256];
    uint16_t writes;
    uint16_t reads;
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, uint32_t memory,
                          const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (memory + length > sizeof(fake->memory)) return 0;
    memcpy(&fake->memory[memory], data, length);
    fake->writes++;
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint32_t memory,
                         uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (memory + length > sizeof(fake->memory)) return 0;
    memcpy(data, &fake->memory[memory], length);
    fake->reads++;
    return 1;
}

static void fake_delay(void *context, uint16_t milliseconds)
{
    fake_t *fake = (fake_t *)context;
    (void)milliseconds;
    fake->delays++;
}

int main(void)
{
    fake_t fake;
    at24cxx_t eeprom;
    at24cxx_port_t port;
    uint8_t data[10];
    uint8_t back[10];

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write_at = fake_write;
    port.read_at = fake_read;
    port.delay_ms = fake_delay;
    CHECK(at24cxx_init(&eeprom, port, 0x50, AT24C02) == 1);
    CHECK(eeprom.capacity == 256 && eeprom.page_size == 8 && eeprom.address_bytes == 1);

    for (uint8_t i = 0; i < sizeof(data); i++) data[i] = (uint8_t)(0xA0U + i);
    CHECK(at24cxx_write(&eeprom, 0xF4, data, sizeof(data)) == 1);
    CHECK(fake.writes == 2);
    CHECK(fake.delays == 2);
    CHECK(fake.memory[0xF4] == 0xA0 && fake.memory[0xF7] == 0xA3);
    CHECK(fake.memory[0xF8] == 0xA4 && fake.memory[0xFD] == 0xA9);

    CHECK(at24cxx_read(&eeprom, 0xF4, back, sizeof(back)) == 1);
    CHECK(memcmp(data, back, sizeof(data)) == 0);
    CHECK(fake.reads == 1);

    CHECK(at24cxx_write(&eeprom, 0x100, data, 1) == 0);
    CHECK(at24cxx_write(&eeprom, 0xFC, data, 5) == 0);
    CHECK(at24cxx_read(&eeprom, 0, back, 0) == 0);

    if (failures) return 1;
    puts("at24cxx: ok");
    return 0;
}
