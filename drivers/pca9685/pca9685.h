#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} pca9685_port_t;

typedef struct {
    pca9685_port_t port;
    uint8_t i2c_address;
} pca9685_t;

uint8_t pca9685_init(pca9685_t *device, pca9685_port_t port,
                     uint8_t i2c_address, uint16_t frequency_hz);
uint8_t pca9685_set_pwm(pca9685_t *device, uint8_t channel,
                        uint16_t on_count, uint16_t off_count);
uint8_t pca9685_set_all(pca9685_t *device, uint16_t on_count, uint16_t off_count);
uint8_t pca9685_set_frequency(pca9685_t *device, uint16_t frequency_hz);

#ifdef __cplusplus
}
#endif

#endif
