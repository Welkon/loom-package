#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADS1115_RANGE_6P144V = 0,
    ADS1115_RANGE_4P096V,
    ADS1115_RANGE_2P048V,
    ADS1115_RANGE_1P024V,
    ADS1115_RANGE_0P512V,
    ADS1115_RANGE_0P256V,
} ads1115_range_t;

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, uint8_t reg,
                     const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t reg,
                    uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} ads1115_port_t;

typedef struct {
    ads1115_port_t port;
    uint8_t i2c_address;
} ads1115_t;

uint8_t ads1115_init(ads1115_t *device, ads1115_port_t port, uint8_t i2c_address);
uint8_t ads1115_read_single(ads1115_t *device, uint8_t channel,
                            ads1115_range_t range, int16_t *raw, int32_t *microvolts);

#ifdef __cplusplus
}
#endif

#endif
