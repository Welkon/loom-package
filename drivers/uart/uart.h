#ifndef UART_H
#define UART_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UART_OK = 0,
    UART_TIMEOUT = 1
} uart_status_t;

typedef struct {
    uart_port_t port;
    uint32_t timeout_us;
} uart_t;

void uart_init(uart_t *uart, uart_port_t port, uint32_t timeout_us);
void uart_write(uart_t *uart, const uint8_t *data, uint16_t length);
uart_status_t uart_read(uart_t *uart, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
