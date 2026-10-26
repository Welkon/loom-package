#include "mcp2515.h"

#define INSTRUCTION_RESET 0xC0U
#define INSTRUCTION_READ 0x03U
#define INSTRUCTION_WRITE 0x02U
#define INSTRUCTION_BIT_MODIFY 0x05U
#define INSTRUCTION_RTS_TXB0 0x81U

#define REG_CANSTAT 0x0EU
#define REG_CANCTRL 0x0FU
#define REG_CNF3 0x28U
#define REG_CNF2 0x29U
#define REG_CNF1 0x2AU
#define REG_CANINTE 0x2BU
#define REG_CANINTF 0x2CU
#define REG_EFLG 0x2DU
#define REG_TXB0CTRL 0x30U
#define REG_TXB0SIDH 0x31U
#define REG_RXB0CTRL 0x60U
#define REG_RXB0SIDH 0x61U
#define REG_RXB1CTRL 0x70U
#define REG_RXB1SIDH 0x71U

#define CANINTF_RX0IF 0x01U
#define CANINTF_RX1IF 0x02U
#define MODE_NORMAL 0x00U
#define MODE_CONFIG 0x80U

typedef struct {
    uint8_t clock_mhz;
    uint16_t bitrate_kbps;
    uint8_t cnf1;
    uint8_t cnf2;
    uint8_t cnf3;
} bitrate_entry_t;

static const bitrate_entry_t bitrates[] = {
    { 8,  125, 0x01, 0xB1, 0x85},
    { 8,  250, 0x00, 0xB1, 0x85},
    { 8,  500, 0x00, 0x90, 0x82},
    { 8, 1000, 0x00, 0x80, 0x80},
    {16,  125, 0x03, 0xF0, 0x86},
    {16,  250, 0x41, 0xF1, 0x85},
    {16,  500, 0x00, 0xF0, 0x86},
    {16, 1000, 0x00, 0xD0, 0x82},
    {20,  125, 0x03, 0xFA, 0x87},
    {20,  250, 0x41, 0xFB, 0x86},
    {20,  500, 0x00, 0xFA, 0x87},
    {20, 1000, 0x00, 0xD9, 0x82},
};

static uint8_t read_reg(mcp2515_t *device, uint8_t reg)
{
    uint8_t value;
    device->port.select(device->port.context);
    (void)device->port.transfer(device->port.context, INSTRUCTION_READ);
    (void)device->port.transfer(device->port.context, reg);
    value = device->port.transfer(device->port.context, 0x00U);
    device->port.deselect(device->port.context);
    return value;
}

static void write_reg(mcp2515_t *device, uint8_t reg, uint8_t value)
{
    device->port.select(device->port.context);
    (void)device->port.transfer(device->port.context, INSTRUCTION_WRITE);
    (void)device->port.transfer(device->port.context, reg);
    (void)device->port.transfer(device->port.context, value);
    device->port.deselect(device->port.context);
}

static void write_regs(mcp2515_t *device, uint8_t reg, const uint8_t *data, uint16_t length)
{
    uint16_t i;
    device->port.select(device->port.context);
    (void)device->port.transfer(device->port.context, INSTRUCTION_WRITE);
    (void)device->port.transfer(device->port.context, reg);
    for (i = 0; i < length; i++) {
        (void)device->port.transfer(device->port.context, data[i]);
    }
    device->port.deselect(device->port.context);
}

static void read_regs(mcp2515_t *device, uint8_t reg, uint8_t *data, uint16_t length)
{
    uint16_t i;
    device->port.select(device->port.context);
    (void)device->port.transfer(device->port.context, INSTRUCTION_READ);
    (void)device->port.transfer(device->port.context, reg);
    for (i = 0; i < length; i++) {
        data[i] = device->port.transfer(device->port.context, 0x00U);
    }
    device->port.deselect(device->port.context);
}

static void bit_modify(mcp2515_t *device, uint8_t reg, uint8_t mask, uint8_t value)
{
    device->port.select(device->port.context);
    (void)device->port.transfer(device->port.context, INSTRUCTION_BIT_MODIFY);
    (void)device->port.transfer(device->port.context, reg);
    (void)device->port.transfer(device->port.context, mask);
    (void)device->port.transfer(device->port.context, value);
    device->port.deselect(device->port.context);
}

uint8_t mcp2515_init(mcp2515_t *device, mcp2515_port_t port,
                     uint8_t clock_mhz, uint16_t bitrate_kbps)
{
    const bitrate_entry_t *entry = 0;
    uint8_t i;
    if (device == 0 || (clock_mhz != 8U && clock_mhz != 16U && clock_mhz != 20U)) {
        return 0;
    }
    for (i = 0; i < sizeof(bitrates) / sizeof(bitrates[0]); i++) {
        if (bitrates[i].clock_mhz == clock_mhz && bitrates[i].bitrate_kbps == bitrate_kbps) {
            entry = &bitrates[i];
            break;
        }
    }
    if (entry == 0) {
        return 0;
    }
    device->port = port;
    device->clock_mhz = clock_mhz;
    device->bitrate_kbps = bitrate_kbps;

    port.select(port.context);
    (void)port.transfer(port.context, INSTRUCTION_RESET);
    port.deselect(port.context);
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 10U);
    }

    write_reg(device, REG_CNF3, entry->cnf3);
    write_reg(device, REG_CNF2, entry->cnf2);
    write_reg(device, REG_CNF1, entry->cnf1);
    write_reg(device, REG_RXB0CTRL, 0x64U);
    write_reg(device, REG_RXB1CTRL, 0x60U);
    write_reg(device, REG_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF);
    write_reg(device, REG_CANINTF, 0x00U);
    write_reg(device, REG_EFLG, 0x00U);
    bit_modify(device, REG_CANCTRL, 0xE0U, MODE_NORMAL);
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 1U);
    }
    return 1;
}

uint8_t mcp2515_send(mcp2515_t *device, const can_frame_t *frame)
{
    uint8_t data[13];
    uint8_t length;
    uint32_t id;
    if (device == 0 || frame == 0 || frame->dlc > 8U) {
        return 0;
    }
    id = frame->id & (frame->extended ? 0x1FFFFFFFUL : 0x7FFUL);
    if (frame->extended) {
        data[0] = (uint8_t)(id >> 21);
        data[1] = (uint8_t)(((id >> 18) & 0xE0U) | 0x08U | ((id >> 16) & 0x03U));
        data[2] = (uint8_t)(id >> 8);
        data[3] = (uint8_t)id;
    } else {
        data[0] = (uint8_t)(id >> 3);
        data[1] = (uint8_t)((id & 0x07U) << 5);
        data[2] = 0x00U;
        data[3] = 0x00U;
    }
    data[4] = (uint8_t)(frame->dlc & 0x0FU);
    for (length = 0; length < frame->dlc; length++) {
        data[5U + length] = frame->data[length];
    }
    write_regs(device, REG_TXB0SIDH, data, (uint16_t)(5U + frame->dlc));
    device->port.select(device->port.context);
    (void)device->port.transfer(device->port.context, INSTRUCTION_RTS_TXB0);
    device->port.deselect(device->port.context);
    return 1;
}

uint8_t mcp2515_receive(mcp2515_t *device, can_frame_t *frame)
{
    uint8_t flags;
    uint8_t base;
    uint8_t data[13];
    uint8_t dlc;
    uint8_t i;
    uint32_t id;
    if (device == 0 || frame == 0) {
        return 0;
    }
    flags = read_reg(device, REG_CANINTF);
    if ((flags & CANINTF_RX0IF) != 0U) {
        base = REG_RXB0SIDH;
    } else if ((flags & CANINTF_RX1IF) != 0U) {
        base = REG_RXB1SIDH;
    } else {
        return 0;
    }
    read_regs(device, base, data, sizeof(data));
    dlc = (uint8_t)(data[4] & 0x0FU);
    if (dlc > 8U) {
        dlc = 8U;
    }
    if ((data[1] & 0x08U) != 0U) {
        id = ((uint32_t)data[0] << 21)
            | ((uint32_t)(data[1] >> 5) << 18)
            | ((uint32_t)(data[1] & 0x03U) << 16)
            | ((uint32_t)data[2] << 8)
            | data[3];
        frame->extended = 1;
    } else {
        id = ((uint32_t)data[0] << 3) | ((uint32_t)data[1] >> 5);
        frame->extended = 0;
    }
    frame->id = id;
    frame->dlc = dlc;
    for (i = 0; i < dlc; i++) {
        frame->data[i] = data[5U + i];
    }
    bit_modify(device, REG_CANINTF, (uint8_t)(CANINTF_RX0IF | CANINTF_RX1IF), 0x00U);
    return 1;
}
