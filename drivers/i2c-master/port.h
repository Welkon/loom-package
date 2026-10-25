#ifndef I2C_PORT_H
#define I2C_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*start)(void *context);
    void (*stop)(void *context);
    uint8_t (*write)(void *context, uint8_t byte);
    uint8_t (*read)(void *context, uint8_t ack);
    uint32_t (*now_us)(void *context);
} i2c_port_t;

#ifdef __cplusplus
}
#endif

#endif
