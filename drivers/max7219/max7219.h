#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*select)(void *context);
    uint8_t (*transfer)(void *context, uint8_t value);
    void (*deselect)(void *context);
} max7219_port_t;

typedef struct {
    max7219_port_t port;
    uint8_t devices;
} max7219_t;

void max7219_init(max7219_t *driver, max7219_port_t port, uint8_t devices);
void max7219_write(max7219_t *driver, uint8_t device, uint8_t reg, uint8_t value);
void max7219_set_digit(max7219_t *driver, uint8_t device, uint8_t digit, uint8_t value);
void max7219_set_row(max7219_t *driver, uint8_t device, uint8_t row, uint8_t value);
void max7219_set_intensity(max7219_t *driver, uint8_t device, uint8_t value);
void max7219_shutdown(max7219_t *driver, uint8_t device, uint8_t shutdown);
void max7219_clear(max7219_t *driver, uint8_t device);

#ifdef __cplusplus
}
#endif

#endif
