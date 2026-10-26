#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *context;
    void (*select)(void *context);
    uint8_t (*transfer)(void *context, uint8_t value);
    void (*deselect)(void *context);
    void (*reset)(void *context, uint8_t level);
    void (*delay_ms)(void *context, uint16_t milliseconds);
} mfrc522_port_t;

typedef struct {
    mfrc522_port_t port;
} mfrc522_t;

uint8_t mfrc522_init(mfrc522_t *reader, mfrc522_port_t port);
uint8_t mfrc522_request(mfrc522_t *reader, uint16_t *atqa);
uint8_t mfrc522_anticoll(mfrc522_t *reader, uint8_t uid[5]);
uint8_t mfrc522_read_uid(mfrc522_t *reader, uint8_t uid[4]);

#ifdef __cplusplus
}
#endif

#endif
