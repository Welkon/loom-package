#include "nrf24l01.h"

#define REG_CONFIG 0x00U
#define REG_EN_AA 0x01U
#define REG_EN_RXADDR 0x02U
#define REG_SETUP_AW 0x03U
#define REG_SETUP_RETR 0x04U
#define REG_RF_CH 0x05U
#define REG_RF_SETUP 0x06U
#define REG_STATUS 0x07U
#define REG_RX_ADDR_P0 0x0AU
#define REG_TX_ADDR 0x10U
#define REG_RX_PW_P0 0x11U

#define CMD_R_REGISTER 0x00U
#define CMD_W_REGISTER 0x20U
#define CMD_R_RX_PAYLOAD 0x61U
#define CMD_W_TX_PAYLOAD 0xA0U
#define CMD_FLUSH_TX 0xE1U
#define CMD_FLUSH_RX 0xE2U
#define STATUS_RX_DR 0x40U
#define STATUS_TX_DS 0x20U
#define STATUS_MAX_RT 0x10U

static uint8_t read_reg(nrf24l01_t *radio, uint8_t reg)
{
    uint8_t value;
    radio->port.select(radio->port.context);
    (void)radio->port.transfer(radio->port.context, (uint8_t)(CMD_R_REGISTER | reg));
    value = radio->port.transfer(radio->port.context, 0x00U);
    radio->port.deselect(radio->port.context);
    return value;
}

static void write_reg(nrf24l01_t *radio, uint8_t reg, uint8_t value)
{
    radio->port.select(radio->port.context);
    (void)radio->port.transfer(radio->port.context, (uint8_t)(CMD_W_REGISTER | reg));
    (void)radio->port.transfer(radio->port.context, value);
    radio->port.deselect(radio->port.context);
}

static void write_buf(nrf24l01_t *radio, uint8_t command, const uint8_t *data, uint8_t length)
{
    uint8_t i;
    radio->port.select(radio->port.context);
    (void)radio->port.transfer(radio->port.context, command);
    for (i = 0; i < length; i++) {
        (void)radio->port.transfer(radio->port.context, data[i]);
    }
    radio->port.deselect(radio->port.context);
}

static void read_buf(nrf24l01_t *radio, uint8_t command, uint8_t *data, uint8_t length)
{
    uint8_t i;
    radio->port.select(radio->port.context);
    (void)radio->port.transfer(radio->port.context, command);
    for (i = 0; i < length; i++) {
        data[i] = radio->port.transfer(radio->port.context, 0x00U);
    }
    radio->port.deselect(radio->port.context);
}

static void flush(nrf24l01_t *radio, uint8_t command)
{
    radio->port.select(radio->port.context);
    (void)radio->port.transfer(radio->port.context, command);
    radio->port.deselect(radio->port.context);
}

static void set_mode(nrf24l01_t *radio, uint8_t receive)
{
    uint8_t config = read_reg(radio, REG_CONFIG);
    config = (uint8_t)((config & (uint8_t)~0x01U) | (receive ? 0x01U : 0x00U));
    write_reg(radio, REG_CONFIG, config);
    if (receive) {
        radio->port.ce(radio->port.context, 1U);
    } else {
        radio->port.ce(radio->port.context, 0U);
    }
}

uint8_t nrf24l01_init(nrf24l01_t *radio, nrf24l01_port_t port,
                      uint8_t channel, const uint8_t address[5],
                      uint8_t payload_length)
{
    uint8_t i;
    if (radio == 0 || address == 0 || channel > 125U
        || payload_length == 0U || payload_length > 32U) {
        return 0;
    }
    radio->port = port;
    radio->channel = channel;
    radio->payload_length = payload_length;
    port.ce(port.context, 0U);
    write_reg(radio, REG_CONFIG, 0x0EU); /* CRC, 16-bit, power up, TX */
    write_reg(radio, REG_EN_AA, 0x01U);
    write_reg(radio, REG_EN_RXADDR, 0x01U);
    write_reg(radio, REG_SETUP_AW, 0x03U); /* 5-byte address */
    write_reg(radio, REG_SETUP_RETR, 0x1AU);
    write_reg(radio, REG_RF_CH, channel);
    write_reg(radio, REG_RF_SETUP, 0x06U); /* 1 Mbps, 0 dBm */
    port.select(port.context);
    (void)port.transfer(port.context, (uint8_t)(CMD_W_REGISTER | REG_RX_ADDR_P0));
    for (i = 0; i < 5U; i++) (void)port.transfer(port.context, address[i]);
    port.deselect(port.context);
    port.select(port.context);
    (void)port.transfer(port.context, (uint8_t)(CMD_W_REGISTER | REG_TX_ADDR));
    for (i = 0; i < 5U; i++) (void)port.transfer(port.context, address[i]);
    port.deselect(port.context);
    write_reg(radio, REG_RX_PW_P0, payload_length);
    flush(radio, CMD_FLUSH_TX);
    flush(radio, CMD_FLUSH_RX);
    write_reg(radio, REG_STATUS, 0x70U);
    if (port.delay_ms != 0) port.delay_ms(port.context, 5U);
    return 1;
}

uint8_t nrf24l01_send(nrf24l01_t *radio, const uint8_t *data, uint8_t length)
{
    uint16_t wait;
    uint8_t status;
    if (radio == 0 || data == 0 || length == 0U || length > 32U) {
        return 0;
    }
    set_mode(radio, 0U);
    flush(radio, CMD_FLUSH_TX);
    write_buf(radio, CMD_W_TX_PAYLOAD, data, length);
    radio->port.ce(radio->port.context, 1U);
    if (radio->port.delay_ms != 0) radio->port.delay_ms(radio->port.context, 1U);
    radio->port.ce(radio->port.context, 0U);
    for (wait = 0; wait < 100U; wait++) {
        status = read_reg(radio, REG_STATUS);
        if ((status & STATUS_TX_DS) != 0U) {
            write_reg(radio, REG_STATUS, STATUS_TX_DS);
            flush(radio, CMD_FLUSH_TX);
            return 1;
        }
        if ((status & STATUS_MAX_RT) != 0U) {
            write_reg(radio, REG_STATUS, STATUS_MAX_RT);
            flush(radio, CMD_FLUSH_TX);
            return 0;
        }
        if (radio->port.delay_ms != 0) radio->port.delay_ms(radio->port.context, 1U);
    }
    return 0;
}

uint8_t nrf24l01_receive(nrf24l01_t *radio, uint8_t *data, uint8_t capacity,
                         uint8_t *length)
{
    uint8_t status;
    if (radio == 0 || data == 0 || length == 0 || capacity == 0U) {
        return 0;
    }
    set_mode(radio, 1U);
    status = read_reg(radio, REG_STATUS);
    if ((status & STATUS_RX_DR) == 0U) {
        return 0;
    }
    *length = radio->payload_length < capacity ? radio->payload_length : capacity;
    read_buf(radio, CMD_R_RX_PAYLOAD, data, *length);
    write_reg(radio, REG_STATUS, STATUS_RX_DR);
    flush(radio, CMD_FLUSH_RX);
    return 1;
}
