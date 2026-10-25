# i2c-master

Blocking I2C master core with a port and a fake bus.

## Application scenarios

- Read a sensor register block at startup or on a timer.
- Write a configuration block to an EEPROM or PMIC.
- Drive an OLED or a small display over I2C.
- Probe a bus during manufacturing test.

## Contract

```c
typedef struct {
    void *context;
    void (*start)(void *context);
    void (*stop)(void *context);
    uint8_t (*write)(void *context, uint8_t byte); /* 1 = ACK */
    uint8_t (*read)(void *context, uint8_t ack);   /* ack = 1 */
    uint32_t (*now_us)(void *context);
} i2c_port_t;

i2c_status_t i2c_master_write(i2c_master_t *master, uint8_t address, const uint8_t *data, uint16_t length);
i2c_status_t i2c_master_read(i2c_master_t *master, uint8_t address, uint8_t *data, uint16_t length);
i2c_status_t i2c_master_write_read(i2c_master_t *master, uint8_t address, const uint8_t *write, uint16_t write_length, uint8_t *read, uint16_t read_length);
```

- Address is 7-bit.
- `write` returns 1 for ACK, 0 for NACK.
- `read` sends ACK when `ack` is 1.
- `timeout_us = 0` disables the timeout check.
- The core is blocking; the port owns delays and clock stretching.
- No allocation.

## Parameters

- `timeout_us`: worst-case transaction time; 10..100 ms is typical.
- The port decides bus speed; the core does not set it.

## Memory and timing

- 24 bytes of state plus the port context.
- O(N) bytes; one callback per byte.

## Integration

```c
typedef struct { /* SCL/SDA or hardware handle */ } board_i2c_t;

static void start(void *ctx) { board_i2c_start((board_i2c_t *)ctx); }
static void stop(void *ctx) { board_i2c_stop((board_i2c_t *)ctx); }
static uint8_t write(void *ctx, uint8_t byte) { return board_i2c_write((board_i2c_t *)ctx, byte); }
static uint8_t read(void *ctx, uint8_t ack) { return board_i2c_read((board_i2c_t *)ctx, ack); }
static uint32_t now(void *ctx) { return board_now_us((board_i2c_t *)ctx); }

i2c_master_t bus;
i2c_master_init(&bus, (i2c_port_t){ctx, start, stop, write, read, now}, 20000);
uint8_t reg = 0x10;
uint8_t value[2];
i2c_master_write_read(&bus, 0x50, &reg, 1, value, 2);
```

## Adapters

- Keep SFR, pin mux, clock divider, and bit-bang timing in the adapter.
- Reference examples: Padauk `IIC/IIC_Master`, CMS32 I2C demos.
- The fake port in `tests/` is the host model; do not copy vendor registers into the core.

## When not to use

- From an ISR with a blocking port.
- Multi-master arbitration is required; this core assumes one master.
- The bus needs SMBus/PEC or a full transaction manager; add it above this core.

## Verification

`make test` covers write, NACK, read ACK/NACK, repeated start, timeout, and invalid address.
