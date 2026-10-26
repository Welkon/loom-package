#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*select)(void *context);
    uint8_t (*transfer)(void *context, uint8_t value);
    void (*deselect)(void *context);
    void (*ce)(void *context, uint8_t level);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} nrf24l01_port_t;

typedef struct {
    nrf24l01_port_t port;
    uint8_t channel;
    uint8_t payload_length;
} nrf24l01_t;

uint8_t nrf24l01_init(nrf24l01_t *radio, nrf24l01_port_t port,
                      uint8_t channel, const uint8_t address[5],
                      uint8_t payload_length);
uint8_t nrf24l01_send(nrf24l01_t *radio, const uint8_t *data, uint8_t length);
uint8_t nrf24l01_receive(nrf24l01_t *radio, uint8_t *data, uint8_t capacity,
                         uint8_t *length);

#ifdef __cplusplus
}
#endif

#endif
