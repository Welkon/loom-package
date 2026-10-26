#ifndef HD44780_H
#define HD44780_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*set_rs)(void *context, uint8_t level);
    void (*set_enable)(void *context, uint8_t level);
    void (*write_nibble)(void *context, uint8_t value);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} hd44780_port_t;

typedef struct {
    hd44780_port_t port;
    uint8_t columns;
    uint8_t rows;
} hd44780_t;

uint8_t hd44780_init(hd44780_t *lcd, hd44780_port_t port, uint8_t columns, uint8_t rows);
void hd44780_clear(hd44780_t *lcd);
void hd44780_set_cursor(hd44780_t *lcd, uint8_t column, uint8_t row);
void hd44780_write_char(hd44780_t *lcd, char value);
void hd44780_write_string(hd44780_t *lcd, const char *text);

#ifdef __cplusplus
}
#endif

#endif
