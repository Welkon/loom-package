#ifndef SSD1306_H
#define SSD1306_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

void ssd1306_init(ssd1306_port_t port);
void ssd1306_clear(ssd1306_port_t port);
void ssd1306_set_contrast(ssd1306_port_t port, uint8_t value);
void ssd1306_write_page(ssd1306_port_t port, uint8_t page, uint8_t column,
                        const uint8_t *data, uint8_t width);

#ifdef __cplusplus
}
#endif

#endif
