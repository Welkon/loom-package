#ifndef DS18B20_H
#define DS18B20_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t ds18b20_read(onewire_port_t port, int16_t *temperature_q4);

#ifdef __cplusplus
}
#endif

#endif
