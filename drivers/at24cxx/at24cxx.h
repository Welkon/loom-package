#ifndef AT24CXX_H
#define AT24CXX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AT24C01 = 0,
    AT24C02,
    AT24C04,
    AT24C08,
    AT24C16,
    AT24C32,
    AT24C64,
    AT24C128,
    AT24C256,
    AT24C512,
} at24cxx_type_t;

typedef struct {
    void *context;
    uint8_t (*write_at)(void *context, uint8_t address, uint32_t memory,
                        const uint8_t *data, uint16_t length);
    uint8_t (*read_at)(void *context, uint8_t address, uint32_t memory,
                       uint8_t *data, uint16_t length);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} at24cxx_port_t;

typedef struct {
    at24cxx_port_t port;
    uint8_t i2c_address;
    uint32_t capacity;
    uint16_t page_size;
    uint8_t address_bytes;
} at24cxx_t;

uint8_t at24cxx_init(at24cxx_t *device, at24cxx_port_t port,
                     uint8_t i2c_address, at24cxx_type_t type);
uint8_t at24cxx_read(at24cxx_t *device, uint32_t address,
                     uint8_t *data, uint16_t length);
uint8_t at24cxx_write(at24cxx_t *device, uint32_t address,
                      const uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
