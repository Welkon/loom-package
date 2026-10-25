#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    uint8_t (*write)(void *context, uint8_t address, const uint8_t *data, uint16_t length);
    uint8_t (*read)(void *context, uint8_t address, uint8_t *data, uint16_t length);
} ds3231_port_t;

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
} ds3231_time_t;

uint8_t ds3231_read_time(ds3231_port_t port, ds3231_time_t *time);
uint8_t ds3231_write_time(ds3231_port_t port, const ds3231_time_t *time);
uint8_t ds3231_read_temperature(ds3231_port_t port, int32_t *milli_celsius);
uint8_t ds3231_lost_power(ds3231_port_t port, uint8_t *lost);

#ifdef __cplusplus
}
#endif

#endif
