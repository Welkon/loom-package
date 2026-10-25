#include <stdio.h>
#include <string.h>
#include "crc32.h"

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
    uint32_t streamed = 0xFFFFFFFFU;
    size_t index;

    CHECK(crc32((const uint8_t *)text, strlen(text)) == 0xCBF43926U);
    for (index = 0; index < strlen(text); index++) {
        streamed = crc32_update(streamed, (uint8_t)text[index]);
    }
    CHECK((streamed ^ 0xFFFFFFFFU) == 0xCBF43926U);
    CHECK(crc32((const uint8_t *)"", 0) == 0U);

    if (failures) return 1;
    puts("crc32: ok");
    return 0;
}
