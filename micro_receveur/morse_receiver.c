#include "morse_receiver.h"
#include "config.h"
#include <stddef.h>

MorseStatus morse_receiver_init(MorseReceiver *rcv)
{
    if (rcv == NULL) return MORSE_ERR_NULL;

    rcv->state          = RECEIVER_IDLE;
    rcv->timestamp_ms   = 0;
    rcv->debounce_count = 0;

    morse_decoder_init(&rcv->decoder);
    return morse_table_load(&rcv->table);
}

void morse_receiver_update(MorseReceiver *rcv,
                           uint16_t       adc_value,
                           uint32_t       now_ms)
{
    if (rcv == NULL) return;
    if (morse_receiver_is_done(rcv)) return;

    uint8_t is_active = (adc_value >= MORSE_ADC_THRESHOLD);

    switch (rcv->state) {

        case RECEIVER_IDLE:
        case RECEIVER_SILENT:
            if (is_active) {
                rcv->debounce_count++;
                if (rcv->debounce_count >= MORSE_DEBOUNCE_SAMPLES) {

                    /* Transition silence → actif confirmée */
                    uint32_t silence_duration = now_ms - rcv->timestamp_ms;
                    MorseTimingClass cls =
                        morse_classify_duration(silence_duration, 0);
                    morse_decoder_push(&rcv->decoder, &rcv->table, cls);

                    rcv->state          = RECEIVER_ACTIVE;
                    rcv->timestamp_ms   = now_ms;
                    rcv->debounce_count = 0;
                }
            } else {
                rcv->debounce_count = 0;
            }
            break;

        case RECEIVER_ACTIVE:
            if (!is_active) {
                rcv->debounce_count++;
                if (rcv->debounce_count >= MORSE_DEBOUNCE_SAMPLES) {

                    /* Transition actif → silence confirmée */
                    uint32_t signal_duration = now_ms - rcv->timestamp_ms;
                    MorseTimingClass cls =
                        morse_classify_duration(signal_duration, 1);
                    morse_decoder_push(&rcv->decoder, &rcv->table, cls);

                    rcv->state          = RECEIVER_SILENT;
                    rcv->timestamp_ms   = now_ms;
                    rcv->debounce_count = 0;
                }
            } else {
                rcv->debounce_count = 0;
            }
            break;
    }
}

uint8_t morse_receiver_is_done(const MorseReceiver *rcv)
{
    if (rcv == NULL) return 0;
    return (rcv->decoder.state == DECODER_DONE);
}

const char *morse_receiver_get_message(const MorseReceiver *rcv)
{
    if (rcv == NULL) return NULL;
    return morse_decoder_get_message(&rcv->decoder);
}