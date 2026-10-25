# ds3231

DS3231 I2C real-time clock.

## Application scenarios

- Timestamps for a data logger.
- A wall clock on an instrument.
- Scheduled actions that survive a reset.

## Contract

```c
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
```

- Time is 24-hour; `year` is 2000..2199.
- `read_temperature` returns millidegrees Celsius (0.25 resolution).
- `lost_power` reports the oscillator-stop flag.
- Returns 1 on success, 0 on a bus error or invalid BCD.

## Parameters

- I2C address `0x68`.
- Registers 0x00..0x06 hold time; 0x0F status; 0x11/0x12 temperature.
- A backup cell keeps the oscillator running.

## Memory and timing

- No allocation; one 7-byte read or 8-byte write.
- Set the time once, then read it at the rate the application needs.

## Integration

```c
ds3231_time_t now;
if (ds3231_read_time(rtc_port(), &now)) {
    log_time(now.year, now.month, now.date, now.hour, now.minute, now.second);
}
```

## Adapters

- The adapter issues a register write followed by a repeated-start read.
- Reference: Adafruit RTClib `RTC_DS3231` (MIT) and the DS3231 datasheet.
- This implementation is original.

## When not to use

- A radio or GPS time source is available and no backup is needed.
- Sub-second timestamps; add a monotonic timer.

## Verification

`make test` covers BCD round-trip, 24-hour time, century, negative temperature,
and the oscillator-stop flag.
