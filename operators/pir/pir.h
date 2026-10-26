#ifndef PIR_H
#define PIR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIR_WINDOW_SAMPLES 6
#define PIR_BATCH_SAMPLES 6
#define PIR_FIRST_BATCHES 16
#define PIR_JAM_DELTA 400
#define PIR_OUTPUT_LOCK_MS 3000
#define PIR_ERROR_LOCK_MS 5000

typedef struct {
    uint16_t window[PIR_WINDOW_SAMPLES];
    uint16_t minimum;
    uint16_t maximum;
    uint32_t sum;
    uint32_t lock_start;
    uint16_t lock_duration;
    uint16_t motion_span;
    uint16_t last_sample;
    uint8_t count;
    uint8_t window_count;
    uint8_t first_batches;
    uint8_t warm;
    uint8_t jam;
    uint8_t locked;
} pir_t;

void pir_init(pir_t *pir, uint16_t motion_span);
uint8_t pir_step(pir_t *pir, uint16_t sample, uint32_t now_ms);
void pir_set_warm(pir_t *pir, uint8_t warm);
void pir_output_changed(pir_t *pir, uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
