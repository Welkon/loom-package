#include "pir.h"

static uint8_t elapsed(uint32_t now, uint32_t since, uint32_t duration)
{
    return (uint32_t)(now - since) >= duration;
}

void pir_init(pir_t *pir, uint16_t motion_span)
{
    uint8_t i;
    if (pir == 0) {
        return;
    }
    for (i = 0; i < PIR_WINDOW_SAMPLES; i++) {
        pir->window[i] = 0;
    }
    pir->minimum = 0;
    pir->maximum = 0;
    pir->sum = 0;
    pir->lock_start = 0;
    pir->lock_duration = 0;
    pir->motion_span = motion_span;
    pir->last_sample = 0;
    pir->count = 0;
    pir->window_count = 0;
    pir->first_batches = 0;
    pir->warm = 0;
    pir->jam = 0;
    pir->locked = 0;
}

void pir_set_warm(pir_t *pir, uint8_t warm)
{
    if (pir != 0) {
        pir->warm = warm ? 1U : 0U;
    }
}

void pir_output_changed(pir_t *pir, uint32_t now_ms)
{
    if (pir != 0) {
        pir->lock_start = now_ms;
        pir->lock_duration = PIR_OUTPUT_LOCK_MS;
        pir->locked = 1;
    }
}

uint8_t pir_step(pir_t *pir, uint16_t sample, uint32_t now_ms)
{
    uint16_t delta;
    uint16_t average;
    uint16_t minimum;
    uint16_t maximum;
    uint8_t i;

    if (pir == 0) {
        return 0;
    }
    delta = sample > pir->last_sample ? (uint16_t)(sample - pir->last_sample)
                                      : (uint16_t)(pir->last_sample - sample);
    if (delta > PIR_JAM_DELTA) {
        if (pir->jam) {
            pir->lock_start = now_ms;
            pir->lock_duration = PIR_ERROR_LOCK_MS;
            pir->locked = 1;
        }
        pir->jam = 1;
    } else {
        pir->jam = 0;
    }
    pir->last_sample = sample;

    if (pir->count == 0U) {
        pir->minimum = sample;
        pir->maximum = sample;
    }
    if (sample < pir->minimum) {
        pir->minimum = sample;
    }
    if (sample > pir->maximum) {
        pir->maximum = sample;
    }
    pir->sum += sample;
    if (++pir->count < PIR_BATCH_SAMPLES) {
        return 0;
    }
    average = (uint16_t)((pir->sum - pir->minimum - pir->maximum) >> 2);
    pir->sum = 0;
    pir->count = 0;
    for (i = 1; i < PIR_WINDOW_SAMPLES; i++) {
        pir->window[i - 1U] = pir->window[i];
    }
    pir->window[PIR_WINDOW_SAMPLES - 1U] = average;
    if (pir->window_count < PIR_WINDOW_SAMPLES) {
        pir->window_count++;
    }
    if (pir->first_batches < PIR_FIRST_BATCHES) {
        pir->first_batches++;
        return 0;
    }
    if (!pir->warm || pir->jam) {
        return 0;
    }
    if (pir->locked) {
        if (!elapsed(now_ms, pir->lock_start, pir->lock_duration)) {
            return 0;
        }
        pir->locked = 0;
    }
    if (pir->window_count < PIR_WINDOW_SAMPLES) {
        return 0;
    }
    minimum = maximum = pir->window[0];
    for (i = 1; i < PIR_WINDOW_SAMPLES; i++) {
        if (pir->window[i] < minimum) {
            minimum = pir->window[i];
        }
        if (pir->window[i] > maximum) {
            maximum = pir->window[i];
        }
    }
    if ((uint16_t)(maximum - minimum) <= pir->motion_span) {
        return 0;
    }
    if ((pir->window[5] > pir->window[4] && pir->window[3] > pir->window[2]
         && pir->window[1] > pir->window[0])
        || (pir->window[5] < pir->window[4] && pir->window[3] < pir->window[2]
            && pir->window[1] < pir->window[0])) {
        return 1;
    }
    return 0;
}
