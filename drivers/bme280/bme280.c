#include "bme280.h"

#define REG_ID 0xD0U
#define REG_RESET 0xE0U
#define REG_CTRL_HUM 0xF2U
#define REG_CTRL_MEAS 0xF4U
#define REG_CONFIG 0xF5U
#define REG_DATA 0xF7U

static uint16_t u16(const uint8_t *data)
{
    return (uint16_t)(data[0] | ((uint16_t)data[1] << 8));
}

static int16_t s16(const uint8_t *data)
{
    return (int16_t)u16(data);
}

static int16_t s12(uint16_t value)
{
    return (int16_t)((value & 0x0800U) ? (value - 0x1000U) : value);
}

static uint8_t write_reg(bme280_t *device, uint8_t reg, uint8_t value)
{
    return device->port.write(device->port.context, device->i2c_address,
                              reg, &value, 1U) ? 1U : 0U;
}

static uint8_t read_regs(bme280_t *device, uint8_t reg, uint8_t *data, uint16_t length)
{
    return device->port.read(device->port.context, device->i2c_address,
                             reg, data, length) ? 1U : 0U;
}

static uint8_t load_calibration(bme280_t *device)
{
    uint8_t data[26];
    uint8_t humidity[7];
    uint16_t h4;
    uint16_t h5;
    if (!read_regs(device, 0x88U, data, sizeof(data))
        || !read_regs(device, 0xE1U, humidity, sizeof(humidity))) {
        return 0;
    }
    device->t1 = u16(&data[0]);
    device->t2 = s16(&data[2]);
    device->t3 = s16(&data[4]);
    device->p1 = u16(&data[6]);
    device->p2 = s16(&data[8]);
    device->p3 = s16(&data[10]);
    device->p4 = s16(&data[12]);
    device->p5 = s16(&data[14]);
    device->p6 = s16(&data[16]);
    device->p7 = s16(&data[18]);
    device->p8 = s16(&data[20]);
    device->p9 = s16(&data[22]);
    device->h1 = data[25];
    device->h2 = s16(&humidity[0]);
    device->h3 = humidity[2];
    h4 = (uint16_t)(((uint16_t)humidity[3] << 4) | (humidity[4] & 0x0FU));
    h5 = (uint16_t)(((uint16_t)humidity[5] << 4) | (humidity[4] >> 4));
    device->h4 = s12(h4);
    device->h5 = s12(h5);
    device->h6 = (int8_t)humidity[6];
    return 1;
}

uint8_t bme280_init(bme280_t *device, bme280_port_t port, uint8_t i2c_address)
{
    uint8_t identity;
    if (device == 0) {
        return 0;
    }
    device->port = port;
    device->i2c_address = i2c_address;
    if (!read_regs(device, REG_ID, &identity, 1U) || identity != 0x60U) {
        return 0;
    }
    if (!write_reg(device, REG_RESET, 0xB6U)) {
        return 0;
    }
    if (port.delay_ms != 0) {
        port.delay_ms(port.context, 5U);
    }
    if (!load_calibration(device)) {
        return 0;
    }
    return write_reg(device, REG_CTRL_HUM, 0x01U)
        && write_reg(device, REG_CONFIG, 0xA0U)
        && write_reg(device, REG_CTRL_MEAS, 0x25U);
}

static int32_t compensate_temperature(bme280_t *device, int32_t raw)
{
    int32_t var1 = ((((raw >> 3) - ((int32_t)device->t1 << 1))) * device->t2) >> 11;
    int32_t var2 = (((((raw >> 4) - (int32_t)device->t1)
                      * ((raw >> 4) - (int32_t)device->t1)) >> 12) * device->t3) >> 14;
    int32_t temperature;
    device->t_fine = var1 + var2;
    temperature = (device->t_fine * 5 + 128) >> 8;
    if (temperature < -4000) {
        temperature = -4000;
    } else if (temperature > 8500) {
        temperature = 8500;
    }
    return temperature;
}

static uint32_t compensate_pressure(bme280_t *device, int32_t raw)
{
    int64_t var1 = (int64_t)device->t_fine - 128000;
    int64_t var2 = var1 * var1 * (int64_t)device->p6;
    int64_t var3;
    int64_t var4;
    uint32_t pressure;
    var2 += (var1 * (int64_t)device->p5) * 131072;
    var2 += (int64_t)device->p4 * 34359738368;
    var1 = ((var1 * var1 * (int64_t)device->p3) / 256)
         + (var1 * (int64_t)device->p2 * 4096);
    var3 = INT64_C(140737488355328);
    var1 = (var3 + var1) * (int64_t)device->p1 / 8589934592;
    if (var1 == 0) {
        return 3000000U;
    }
    var4 = 1048576 - raw;
    var4 = (((var4 * INT64_C(2147483648)) - var2) * 3125) / var1;
    var3 = ((int64_t)device->p9 * (var4 / 8192) * (var4 / 8192)) / 33554432;
    var2 = ((int64_t)device->p8 * var4) / 524288;
    var4 = ((var4 + var3 + var2) / 256) + ((int64_t)device->p7 * 16);
    pressure = (uint32_t)(((var4 / 2) * 100) / 128);
    if (pressure < 3000000U) {
        pressure = 3000000U;
    } else if (pressure > 11000000U) {
        pressure = 11000000U;
    }
    return pressure;
}

static uint32_t compensate_humidity(bme280_t *device, int32_t raw)
{
    int32_t var1 = device->t_fine - 76800;
    int32_t var2 = raw * 16384;
    int32_t var3 = (int32_t)device->h4 * 1048576;
    int32_t var4 = (int32_t)device->h5 * var1;
    int32_t var5 = (((var2 - var3) - var4) + 16384) / 32768;
    uint32_t humidity;
    var2 = (var1 * device->h6) / 1024;
    var3 = (var1 * device->h3) / 2048;
    var4 = ((var2 * (var3 + 32768)) / 1024) + 2097152;
    var2 = ((var4 * device->h2) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * device->h1) / 16);
    if (var5 < 0) {
        var5 = 0;
    } else if (var5 > 419430400) {
        var5 = 419430400;
    }
    humidity = (uint32_t)(var5 / 4096);
    if (humidity > 102400U) {
        humidity = 102400U;
    }
    return humidity;
}

uint8_t bme280_read(bme280_t *device, bme280_reading_t *reading)
{
    uint8_t data[8];
    int32_t raw_pressure;
    int32_t raw_temperature;
    int32_t raw_humidity;
    int32_t temperature;
    uint32_t pressure;
    uint32_t humidity;
    if (device == 0 || reading == 0) {
        return 0;
    }
    if (!write_reg(device, REG_CTRL_MEAS, 0x25U)) {
        return 0;
    }
    if (device->port.delay_ms != 0) {
        device->port.delay_ms(device->port.context, 10U);
    }
    if (!read_regs(device, REG_DATA, data, sizeof(data))) {
        return 0;
    }
    raw_pressure = (int32_t)(((uint32_t)data[0] << 12)
                             | ((uint32_t)data[1] << 4)
                             | ((uint32_t)data[2] >> 4));
    raw_temperature = (int32_t)(((uint32_t)data[3] << 12)
                                | ((uint32_t)data[4] << 4)
                                | ((uint32_t)data[5] >> 4));
    raw_humidity = (int32_t)(((uint32_t)data[6] << 8) | data[7]);
    temperature = compensate_temperature(device, raw_temperature);
    pressure = compensate_pressure(device, raw_pressure);
    humidity = compensate_humidity(device, raw_humidity);
    reading->temperature_milli_c = temperature * 10;
    reading->pressure_pa = pressure / 100U;
    reading->humidity_milli_percent = humidity * 1000U / 1024U;
    return 1;
}
