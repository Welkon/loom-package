#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*trigger)(void *context);
    uint32_t (*echo_pulse_us)(void *context);
} hc_sr04_port_t;

uint8_t hc_sr04_read(hc_sr04_port_t port, uint32_t *millimeters);
uint32_t hc_sr04_millimeters(uint32_t echo_us);

#ifdef __cplusplus
}
#endif

#endif
