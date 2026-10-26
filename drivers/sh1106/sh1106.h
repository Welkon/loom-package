#ifndef SH1106_H
#define SH1106_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t control, const uint8_t *data, uint16_t length);
} sh1106_port_t;

void sh1106_init(sh1106_port_t port);
void sh1106_clear(sh1106_port_t port);
void sh1106_set_contrast(sh1106_port_t port, uint8_t value);
void sh1106_write_page(sh1106_port_t port, uint8_t page, uint8_t column,
                       const uint8_t *data, uint8_t width);

#ifdef __cplusplus
}
#endif

#endif
