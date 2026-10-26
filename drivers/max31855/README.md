# max31855

MAX31855 thermocouple amplifier over SPI.

## Application scenarios

- A reflow oven or hot plate.
- A kiln or furnace monitor.
- A thermocouple probe on a test bench.

## Contract

```c
typedef struct {
    int32_t thermocouple_milli_c;
    int32_t internal_milli_c;
    uint8_t fault;
    uint8_t open_circuit;
    uint8_t short_to_gnd;
    uint8_t short_to_vcc;
} max31855_reading_t;

uint8_t max31855_read(max31855_port_t port, max31855_reading_t *reading);
```

- Reads one 32-bit frame.
- Thermocouple resolution is 0.25 degrees C; internal is 0.0625 degrees C.
- Returns 1 when a frame was read, including a fault frame; inspect `fault`.
- Returns 0 on a bus error.

## Parameters

- The thermocouple type (K, J, N, T, S, R, E) is fixed by the chip variant.
- Keep the cold-junction side away from the hot zone.
- Use a 0.1 uF bypass capacitor near the chip.

## Memory and timing

- 8 bytes; one 4-byte SPI transfer.
- Conversion time is about 100 ms.

## Integration

```c
max31855_reading_t temperature;
if (max31855_read(board_max31855_port(), &temperature)) {
    if (temperature.fault) {
        report_fault(temperature.open_circuit, temperature.short_to_gnd);
    } else {
        show(temperature.thermocouple_milli_c / 1000);
    }
}
```

## Adapters

- The adapter drives CS low, reads four bytes, and drives CS high.
- Reference: LibDriver `max31855` (MIT) and the Maxim datasheet.
- This implementation is original.

## When not to use

- A thermistor or RTD is sufficient; use a simpler front end.
- An isolated thermocouple; add an isolator.

## Verification

`make test` covers positive and negative temperatures, the internal junction,
and the three fault flags.
