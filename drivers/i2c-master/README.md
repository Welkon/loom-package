# i2c-master

Blocking I2C master core with a port and a fake bus.

## Contract

```c
typedef struct {
    void *context;
    void (*start)(void *context);
    void (*stop)(void *context);
    uint8_t (*write)(void *context, uint8_t byte);
    uint8_t (*read)(void *context, uint8_t ack);
    uint32_t (*now_us)(void *context);
} i2c_port_t;

i2c_status_t i2c_master_write(i2c_master_t *master, uint8_t address,
                              const uint8_t *data, uint16_t length);
i2c_status_t i2c_master_read(i2c_master_t *master, uint8_t address,
                             uint8_t *data, uint16_t length);
i2c_status_t i2c_master_write_read(i2c_master_t *master, uint8_t address,
                                   const uint8_t *write, uint16_t write_length,
                                   uint8_t *read, uint16_t read_length);
```

- Address is 7-bit.
- `write` returns 1 for ACK, 0 for NACK.
- `read` sends ACK when `ack` is 1.
- `timeout_us = 0` disables the timeout check.
- The core is blocking; the port owns delays and clock stretching.
- No allocation.

## Use

Use it as the protocol core; the port implements GPIO or a hardware
peripheral. Behavior reference: Padauk `IIC/IIC_Master` and CMS32 I2C
examples; this implementation is original.

## Avoid

Do not use it from an ISR with a blocking port. Do not copy a vendor
register sequence into the core; keep it in the adapter.
