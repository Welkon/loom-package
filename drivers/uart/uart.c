#include "uart.h"

void uart_init(uart_t *uart, uart_port_t port, uint32_t timeout_us)
{
    uart->port = port;
    uart->timeout_us = timeout_us;
}

void uart_write(uart_t *uart, const uint8_t *data, uint16_t length)
{
    uint16_t index;
    if (data == 0) {
        return;
    }
    for (index = 0; index < length; index++) {
        uart->port.write(uart->port.context, data[index]);
    }
}

uart_status_t uart_read(uart_t *uart, uint8_t *data, uint16_t length)
{
    uint32_t start;
    uint16_t index;
    if (data == 0) {
        return UART_OK;
    }
    start = uart->port.now_us(uart->port.context);
    for (index = 0; index < length; index++) {
        while (!uart->port.read_ready(uart->port.context)) {
            if (uart->timeout_us != 0U
                && (uint32_t)(uart->port.now_us(uart->port.context) - start) >= uart->timeout_us) {
                return UART_TIMEOUT;
            }
        }
        data[index] = uart->port.read(uart->port.context);
    }
    return UART_OK;
}
