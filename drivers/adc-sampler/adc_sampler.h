#ifndef ADC_SAMPLER_H
#define ADC_SAMPLER_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    adc_port_t port;
    uint8_t samples;
} adc_sampler_t;

void adc_sampler_init(adc_sampler_t *adc, adc_port_t port, uint8_t samples);
uint16_t adc_sampler_read(adc_sampler_t *adc, uint8_t channel);
uint32_t adc_sampler_mv(const adc_sampler_t *adc, uint16_t raw, uint8_t bits, uint32_t vref_mv);

#ifdef __cplusplus
}
#endif

#endif
