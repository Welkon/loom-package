#include "mfrc522.h"

#define REG_COMMAND 0x01U
#define REG_COMIEN 0x02U
#define REG_COMIRQ 0x04U
#define REG_ERROR 0x06U
#define REG_FIFO_DATA 0x09U
#define REG_FIFO_LEVEL 0x0AU
#define REG_BIT_FRAMING 0x0DU
#define REG_MODE 0x11U
#define REG_TX_CONTROL 0x14U
#define REG_TX_ASK 0x15U
#define REG_TMODE 0x2AU
#define REG_TPRESCALER 0x2BU
#define REG_TRELOAD_H 0x2CU
#define REG_TRELOAD_L 0x2DU
#define REG_VERSION 0x37U

#define CMD_IDLE 0x00U
#define CMD_TRANSCEIVE 0x0CU
#define CMD_SOFT_RESET 0x0FU

static void write_reg(mfrc522_t *reader, uint8_t reg, uint8_t value)
{
    reader->port.select(reader->port.context);
    (void)reader->port.transfer(reader->port.context, (uint8_t)((reg << 1) & 0x7EU));
    (void)reader->port.transfer(reader->port.context, value);
    reader->port.deselect(reader->port.context);
}

static uint8_t read_reg(mfrc522_t *reader, uint8_t reg)
{
    uint8_t value;
    reader->port.select(reader->port.context);
    (void)reader->port.transfer(reader->port.context, (uint8_t)(((reg << 1) & 0x7EU) | 0x80U));
    value = reader->port.transfer(reader->port.context, 0x00U);
    reader->port.deselect(reader->port.context);
    return value;
}

static uint8_t transceive(mfrc522_t *reader, const uint8_t *tx, uint8_t tx_length,
                          uint8_t *rx, uint8_t capacity, uint8_t *rx_length,
                          uint8_t framing)
{
    uint8_t i;
    uint8_t level;
    uint16_t wait;
    write_reg(reader, REG_COMMAND, CMD_IDLE);
    write_reg(reader, REG_COMIEN, 0x77U);
    write_reg(reader, REG_FIFO_LEVEL, 0x00U);
    for (i = 0; i < tx_length; i++) {
        write_reg(reader, REG_FIFO_DATA, tx[i]);
    }
    write_reg(reader, REG_BIT_FRAMING, framing);
    write_reg(reader, REG_COMMAND, CMD_TRANSCEIVE);
    for (wait = 0; wait < 100U; wait++) {
        if ((read_reg(reader, REG_COMIRQ) & 0x30U) != 0U) {
            break;
        }
        reader->port.delay_ms(reader->port.context, 1U);
    }
    if (wait == 100U || read_reg(reader, REG_ERROR) != 0U) {
        return 0;
    }
    level = read_reg(reader, REG_FIFO_LEVEL);
    if (level > capacity) {
        return 0;
    }
    for (i = 0; i < level; i++) {
        rx[i] = read_reg(reader, REG_FIFO_DATA);
    }
    *rx_length = level;
    return 1;
}

uint8_t mfrc522_init(mfrc522_t *reader, mfrc522_port_t port)
{
    uint8_t version;
    if (reader == 0) {
        return 0;
    }
    reader->port = port;
    if (port.reset != 0) {
        port.reset(port.context, 1U);
        port.delay_ms(port.context, 1U);
        port.reset(port.context, 0U);
        port.delay_ms(port.context, 1U);
    }
    write_reg(reader, REG_COMMAND, CMD_SOFT_RESET);
    port.delay_ms(port.context, 50U);
    write_reg(reader, REG_TX_ASK, 0x40U);
    write_reg(reader, REG_MODE, 0x3DU);
    write_reg(reader, REG_TMODE, 0x8DU);
    write_reg(reader, REG_TPRESCALER, 0x3EU);
    write_reg(reader, REG_TRELOAD_H, 0x03U);
    write_reg(reader, REG_TRELOAD_L, 0xE8U);
    write_reg(reader, REG_TX_CONTROL, 0x03U);
    version = read_reg(reader, REG_VERSION);
    return (version == 0x91U || version == 0x92U) ? 1U : 0U;
}

uint8_t mfrc522_request(mfrc522_t *reader, uint16_t *atqa)
{
    uint8_t command = 0x26U;
    uint8_t response[2];
    uint8_t length = 0;
    if (reader == 0 || atqa == 0
        || !transceive(reader, &command, 1U, response, sizeof(response), &length, 0x07U)
        || length != 2U) {
        return 0;
    }
    *atqa = (uint16_t)(((uint16_t)response[0] << 8) | response[1]);
    return 1;
}

uint8_t mfrc522_anticoll(mfrc522_t *reader, uint8_t uid[5])
{
    const uint8_t command[2] = {0x93U, 0x20U};
    uint8_t response[5];
    uint8_t length = 0;
    if (reader == 0 || uid == 0
        || !transceive(reader, command, sizeof(command), response, sizeof(response), &length, 0x00U)
        || length != 5U) {
        return 0;
    }
    if ((uint8_t)(response[0] ^ response[1] ^ response[2] ^ response[3]) != response[4]) {
        return 0;
    }
    for (uint8_t i = 0; i < 5U; i++) uid[i] = response[i];
    return 1;
}

uint8_t mfrc522_read_uid(mfrc522_t *reader, uint8_t uid[4])
{
    uint16_t atqa;
    uint8_t full[5];
    if (reader == 0 || uid == 0 || !mfrc522_request(reader, &atqa)
        || !mfrc522_anticoll(reader, full)) {
        return 0;
    }
    for (uint8_t i = 0; i < 4U; i++) uid[i] = full[i];
    return 1;
}
