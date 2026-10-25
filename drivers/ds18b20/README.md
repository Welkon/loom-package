# ds18b20

Read a DS18B20 temperature over a 1-Wire port.

## Application scenarios

- Board or enclosure temperature on a single sensor.
- A probe on a long cable where I2C is inconvenient.
- A low-cost temperature reading with a CRC check.

## Contract

```c
typedef struct {
    void *context;
    uint8_t (*reset)(void *context);
    void (*write_bit)(void *context, uint8_t bit);
    uint8_t (*read_bit)(void *context);
    void (*delay_ms)(void *context, uint16_t ms);
} onewire_port_t;

uint8_t ds18b20_read(onewire_port_t port, int16_t *temperature_q4);
```

- `reset` returns 1 when a presence pulse was seen.
- The port owns the 1-Wire timing: 480 us reset, 60 us slots.
- The result is the raw DS18B20 temperature in 1/16 C (Q4).
- The scratchpad CRC is checked; a bad CRC returns 0.
- No allocation; one device on the bus.

## Parameters

- Conversion takes up to 750 ms at 12-bit resolution; the port's
  `delay_ms` must provide that wait.
- For multiple sensors, add a ROM search layer; this unit uses Skip ROM.

## Memory and timing

- 8 bytes of state plus the caller's 9-byte scratchpad.
- 72 bit slots per byte read, plus the conversion wait.

## Integration

```c
onewire_port_t bus = board_onewire_port();
int16_t raw;
if (ds18b20_read(bus, &raw)) {
    int32_t milli_c = (int32_t)raw * 1000 / 16;
}
```

## Adapters

- Keep GPIO direction changes, delays, and interrupt masking in the adapter.
- Reference: OneWire (MIT) and the DS18B20 datasheet.
- Parasite power needs a strong pull-up; this core assumes normal power.

## When not to use

- A multi-drop bus without a ROM search layer.
- A sensor that needs a different family code or resolution configuration.

## Verification

`make test` covers a valid 25.0625 C scratchpad, CRC failure, no presence, and the command sequence.
