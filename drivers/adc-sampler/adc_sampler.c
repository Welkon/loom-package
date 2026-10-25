#include "adc_sampler.h"

void adc_sampler_init(adc_sampler_t *adc, adc_port_t port, uint8_t samples)
{
    adc->port = port;
    adc->samples = samples == 0U ? 1U : samples;
}

uint16_t adc_sampler_read(adc_sampler_t *adc, uint8_t channel)
{
    uint32_t sum = 0;
    uint8_t index;
    for (index = 0; index < adc->samples; index++) {
        sum += adc->port.read(adc->port.context, channel);
    }
    return (uint16_t)((sum + adc->samples / 2U) / adc->samples);
}

uint32_t adc_sampler_mv(const adc_sampler_t *adc, uint16_t raw, uint8_t bits, uint32_t vref_mv)
{
    uint64_t full_scale;
    (void)adc;
    if (bits == 0U || bits > 16U) {
        return 0;
    }
    full_scale = (uint64_t)1 << bits;
    return (uint32_t)(((uint64_t)raw * vref_mv + full_scale / 2U) / full_scale);
}
