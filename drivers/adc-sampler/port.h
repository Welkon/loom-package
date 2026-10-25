#ifndef ADC_PORT_H
#define ADC_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint16_t (*read)(void *context, uint8_t channel);
} adc_port_t;

#ifdef __cplusplus
}
#endif

#endif
