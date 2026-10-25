#include "ds3231.h"

static uint8_t to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10U) << 4) | (value % 10U));
}

static uint8_t from_bcd(uint8_t value)
{
    uint8_t tens = (uint8_t)(value >> 4);
    uint8_t ones = (uint8_t)(value & 0x0FU);
    if (tens > 9U || ones > 9U) {
        return 0xFFU;
    }
    return (uint8_t)(tens * 10U + ones);
}

uint8_t ds3231_read_time(ds3231_port_t port, ds3231_time_t *time)
{
    uint8_t data[7];
    uint8_t reg = 0x00;
    if (time == 0 || !port.write(port.context, 0x68U, &reg, 1U)
        || !port.read(port.context, 0x68U, data, sizeof(data))) {
        return 0;
    }
    time->second = from_bcd((uint8_t)(data[0] & 0x7FU));
    time->minute = from_bcd(data[1]);
    time->hour = from_bcd((uint8_t)(data[2] & 0x3FU));
    time->day = from_bcd(data[3]);
    time->date = from_bcd(data[4]);
    time->month = from_bcd((uint8_t)(data[5] & 0x1FU));
    time->year = (uint16_t)(2000U + ((data[5] & 0x80U) ? 100U : 0U)
                            + from_bcd(data[6]));
    if (time->second == 0xFFU || time->minute == 0xFFU || time->hour == 0xFFU
        || time->date == 0xFFU || time->month == 0xFFU || time->year > 2199U) {
        return 0;
    }
    return 1;
}

uint8_t ds3231_write_time(ds3231_port_t port, const ds3231_time_t *time)
{
    uint8_t data[8];
    if (time == 0 || time->year < 2000U || time->year > 2199U
        || time->second > 59U || time->minute > 59U || time->hour > 23U
        || time->date < 1U || time->date > 31U || time->month < 1U || time->month > 12U) {
        return 0;
    }
    data[0] = 0x00;
    data[1] = to_bcd(time->second);
    data[2] = to_bcd(time->minute);
    data[3] = to_bcd(time->hour);
    data[4] = to_bcd(time->day);
    data[5] = to_bcd(time->date);
    data[6] = (uint8_t)(to_bcd(time->month) | (time->year >= 2100U ? 0x80U : 0U));
    data[7] = to_bcd((uint8_t)(time->year % 100U));
    if (!port.write(port.context, 0x68U, data, sizeof(data))) {
        return 0;
    }
    data[0] = 0x0F;
    if (!port.write(port.context, 0x68U, data, 1U)
        || !port.read(port.context, 0x68U, data, 1U)) {
        return 0;
    }
    {
        uint8_t status[2] = {0x0F, (uint8_t)(data[0] & 0x7FU)};
        return port.write(port.context, 0x68U, status, 2U) ? 1U : 0U;
    }
}

uint8_t ds3231_read_temperature(ds3231_port_t port, int32_t *milli_celsius)
{
    uint8_t data[2];
    uint8_t reg = 0x11;
    int16_t raw;
    if (milli_celsius == 0 || !port.write(port.context, 0x68U, &reg, 1U)
        || !port.read(port.context, 0x68U, data, sizeof(data))) {
        return 0;
    }
    raw = (int16_t)(((uint16_t)data[0] << 8) | data[1]);
    raw = (int16_t)(raw >> 6);
    *milli_celsius = (int32_t)raw * 250;
    return 1;
}

uint8_t ds3231_lost_power(ds3231_port_t port, uint8_t *lost)
{
    uint8_t reg = 0x0F;
    uint8_t status;
    if (lost == 0 || !port.write(port.context, 0x68U, &reg, 1U)
        || !port.read(port.context, 0x68U, &status, 1U)) {
        return 0;
    }
    *lost = (uint8_t)((status >> 7) & 1U);
    return 1;
}
