#ifndef MAX31855_H
#define MAX31855_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*select)(void *context);
    uint8_t (*transfer)(void *context, uint8_t value);
    void (*deselect)(void *context);
} max31855_port_t;

typedef struct {
    int32_t thermocouple_milli_c;
    int32_t internal_milli_c;
    uint8_t fault;
    uint8_t open_circuit;
    uint8_t short_to_gnd;
    uint8_t short_to_vcc;
} max31855_reading_t;

uint8_t max31855_read(max31855_port_t port, max31855_reading_t *reading);

#ifdef __cplusplus
}
#endif

#endif
