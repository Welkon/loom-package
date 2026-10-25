#include <stdio.h>
#include <string.h>
#include "crc16.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

int main(void)
{
    const char *text = "123456789";
    uint16_t streamed = 0xFFFFU;
    size_t index;

    CHECK(crc16_ccitt((const uint8_t *)text, strlen(text)) == 0x29B1U);
    for (index = 0; index < strlen(text); index++) {
        streamed = crc16_ccitt_update(streamed, (uint8_t)text[index]);
    }
    CHECK(streamed == 0x29B1U);
    CHECK(crc16_ccitt((const uint8_t *)"", 0) == 0xFFFFU);

    if (failures) return 1;
    puts("crc16: ok");
    return 0;
}
