#ifndef SSD1306_PORT_H
#define SSD1306_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t control, const uint8_t *data, uint16_t length);
} ssd1306_port_t;

#ifdef __cplusplus
}
#endif

#endif
