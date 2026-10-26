#include <stdio.h>
#include <string.h>
#include "modbus_rtu.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } \
} while (0)

int main(void)
{
    uint8_t request[8];
    uint8_t response[9];
    uint8_t exception[5] = {0x01, 0x83, 0x02, 0xC0, 0xF1};
    uint16_t values[4];
    uint8_t count;
    uint8_t code;
    uint16_t crc;

    crc = modbus_crc16((const uint8_t[]){0x01, 0x03, 0x00, 0x00, 0x00, 0x0A}, 6);
    CHECK(crc == 0xCDC5);
    CHECK(modbus_build_read(1, 0x03, 0x0000, 10, request) == 8);
    CHECK(request[0] == 0x01 && request[1] == 0x03 && request[6] == 0xC5 && request[7] == 0xCD);

    response[0] = 0x01; response[1] = 0x03; response[2] = 0x04;
    response[3] = 0x00; response[4] = 0x0A;
    response[5] = 0x00; response[6] = 0x0B;
    response[7] = 0x9B; response[8] = 0xF6;
    CHECK(modbus_parse_read(response, sizeof(response), 1, 0x03, values, 4, &count) == 1);
    CHECK(count == 2 && values[0] == 10 && values[1] == 11);

    CHECK(modbus_parse_exception(exception, sizeof(exception), 1, 0x03, &code) == 1);
    CHECK(code == 2);

    response[8] ^= 1U;
    CHECK(modbus_parse_read(response, sizeof(response), 1, 0x03, values, 4, &count) == 0);
    CHECK(modbus_build_read(0, 0x03, 0, 1, request) == 0);
    CHECK(modbus_build_read(1, 0x03, 0, 126, request) == 0);

    if (failures) return 1;
    puts("modbus_rtu: ok");
    return 0;
}
