# dht

DHT11 and DHT22 frame decoders.

## Application scenarios

- Room temperature and humidity on a display.
- Greenhouse or enclosure monitoring.
- A low-cost sensor on a product without a calibrated sensor.

## Contract

```c
typedef struct {
    uint8_t humidity;      /* %RH */
    int8_t temperature;    /* degrees C */
} dht11_reading_t;

typedef struct {
    uint16_t humidity_tenths;   /* 0.1 %RH */
    int16_t temperature_tenths; /* 0.1 degrees C */
} dht22_reading_t;

uint8_t dht11_decode(const uint16_t *high_us, uint8_t count, dht11_reading_t *out);
uint8_t dht22_decode(const uint16_t *high_us, uint8_t count, dht22_reading_t *out);
```

- `high_us` holds the high-pulse widths of the 40 data bits, in order.
- A pulse of at least 50 us is a one; a shorter pulse is a zero.
- `count` must be 40.
- Returns 1 on a valid checksum, 0 otherwise.

## Parameters

- Capture with a 1 us timer or a pin-change interrupt.
- DHT11: humidity and temperature are integers; the fractional bytes are zero.
- DHT22: 16-bit values in tenths; the temperature sign is bit 15.

## Memory and timing

- No state; the caller owns the 40-entry capture array (80 bytes).
- The sensor needs a start pulse and about 4 ms of frame time; keep interrupts short.

## Integration

```c
uint16_t highs[40];
dht11_reading_t reading;
if (dht11_decode(highs, 40, &reading)) {
    show(reading.temperature, reading.humidity);
}
```

## Adapters

- The adapter drives the line low for at least 18 ms, releases it, and captures
  the 40 high pulses.
- Reference: Adafruit DHT sensor library (MIT) and the DHT11/DHT22 datasheets.
- This decoder is original; it does not copy the library.

## When not to use

- A sensor with a standard bus (I2C/SPI) is available.
- The cable is long or the environment is electrically noisy.

## Verification

`make test` covers DHT11 values, DHT22 negative temperatures, a bad checksum,
and a short frame.
