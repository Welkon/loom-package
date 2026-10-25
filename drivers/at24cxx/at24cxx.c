#include "at24cxx.h"

typedef struct {
    uint32_t capacity;
    uint16_t page_size;
    uint8_t address_bytes;
} at24cxx_geometry_t;

static const at24cxx_geometry_t geometries[] = {
    {128,    8, 1},
    {256,    8, 1},
    {512,   16, 1},
    {1024,  16, 1},
    {2048,  16, 1},
    {4096,  32, 2},
    {8192,  32, 2},
    {16384, 64, 2},
    {32768, 64, 2},
    {65536, 128, 2},
};

uint8_t at24cxx_init(at24cxx_t *device, at24cxx_port_t port,
                     uint8_t i2c_address, at24cxx_type_t type)
{
    if (device == 0 || type > AT24C512) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    device->capacity = geometries[type].capacity;
    device->page_size = geometries[type].page_size;
    device->address_bytes = geometries[type].address_bytes;
    return 1;
}

uint8_t at24cxx_read(at24cxx_t *device, uint32_t address,
                     uint8_t *data, uint16_t length)
{
    if (device == 0 || data == 0 || length == 0U
        || address >= device->capacity
        || (uint32_t)length > device->capacity - address) {
        return 0;
    }
    return device->port.read_at(device->port.context, device->i2c_address,
                                address, data, length) ? 1U : 0U;
}

uint8_t at24cxx_write(at24cxx_t *device, uint32_t address,
                      const uint8_t *data, uint16_t length)
{
    uint16_t chunk;
    uint16_t room;
    if (device == 0 || data == 0 || length == 0U
        || address >= device->capacity
        || (uint32_t)length > device->capacity - address) {
        return 0;
    }
    while (length > 0U) {
        room = (uint16_t)(device->page_size - (address % device->page_size));
        chunk = length < room ? length : room;
        if (!device->port.write_at(device->port.context, device->i2c_address,
                                   address, data, chunk)) {
            return 0;
        }
        if (device->port.delay_ms != 0) {
            device->port.delay_ms(device->port.context, 5U);
        }
        address += chunk;
        data += chunk;
        length = (uint16_t)(length - chunk);
    }
    return 1;
}
