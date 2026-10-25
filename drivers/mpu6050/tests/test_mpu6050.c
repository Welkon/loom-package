#include <stdio.h>
#include <string.h>
#include "mpu6050.h"

static int failures = 0;
#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        failures++; \
    } \
} while (0)

typedef struct {
    uint8_t regs[128];
    uint8_t pointer;
    uint16_t delays;
} fake_t;

static uint8_t fake_write(void *context, uint8_t address, const uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    if (length == 0) return 0;
    fake->pointer = data[0];
    for (uint16_t i = 1; i < length; i++) fake->regs[fake->pointer++] = data[i];
    return 1;
}

static uint8_t fake_read(void *context, uint8_t address, uint8_t *data, uint16_t length)
{
    fake_t *fake = (fake_t *)context;
    (void)address;
    for (uint16_t i = 0; i < length; i++) data[i] = fake->regs[fake->pointer++];
    return 1;
}

static void fake_delay(void *context, uint16_t ms)
{
    fake_t *fake = (fake_t *)context;
    (void)ms;
    fake->delays++;
}

int main(void)
{
    fake_t fake;
    mpu6050_t imu;
    mpu6050_port_t port;
    mpu6050_raw_t raw;
    mpu6050_scaled_t scaled;

    memset(&fake, 0, sizeof(fake));
    port.context = &fake;
    port.write = fake_write;
    port.read = fake_read;
    port.delay_ms = fake_delay;
    fake.regs[0x75] = 0x68;

    CHECK(mpu6050_init(&imu, port, 0x68) == 1);
    CHECK(fake.regs[0x6B] == 0x01 && fake.regs[0x19] == 0x07);
    CHECK(fake.regs[0x1A] == 0x03 && fake.regs[0x1B] == 0x00 && fake.regs[0x1C] == 0x00);
    CHECK(fake.delays == 1);

    fake.regs[0x3B] = 0x40; fake.regs[0x3C] = 0x00;
    fake.regs[0x3D] = 0xC0; fake.regs[0x3E] = 0x00;
    fake.regs[0x3F] = 0x00; fake.regs[0x40] = 0x00;
    fake.regs[0x41] = 0x00; fake.regs[0x42] = 0x00;
    fake.regs[0x43] = 0x40; fake.regs[0x44] = 0x00;
    fake.regs[0x45] = 0x80; fake.regs[0x46] = 0x00;
    fake.regs[0x47] = 0x00; fake.regs[0x48] = 0x00;
    CHECK(mpu6050_read_raw(&imu, &raw) == 1);
    CHECK(raw.accel_x == 16384 && raw.accel_y == -16384 && raw.accel_z == 0);
    CHECK(raw.temperature == 0 && raw.gyro_x == 16384 && raw.gyro_y == -32768);
    CHECK(raw.gyro_z == 0);

    CHECK(mpu6050_read_scaled(&imu, &scaled) == 1);
    CHECK(scaled.accel_x_mg == 1000 && scaled.accel_y_mg == -1000);
    CHECK(scaled.gyro_x_mdps == 125000 && scaled.gyro_y_mdps == -250000);
    CHECK(scaled.temperature_milli_celsius == 36530);

    fake.regs[0x75] = 0x70;
    CHECK(mpu6050_init(&imu, port, 0x68) == 0);

    if (failures) return 1;
    puts("mpu6050: ok");
    return 0;
}
