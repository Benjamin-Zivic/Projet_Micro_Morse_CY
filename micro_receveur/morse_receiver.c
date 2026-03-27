#include "morse_receiver.h"
#include "config.h"
#include <stddef.h>
#define MOVING_AVG_SIZE  8

MorseStatus morse_receiver_init(MorseReceiver *rcv)
{
    if (rcv == NULL) return MORSE_ERR_NULL;

    rcv->state          = RECEIVER_IDLE;
    rcv->timestamp_ms   = 0;
    rcv->last_flush_ms  = 0;
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

	    static uint16_t adc_buffer[MOVING_AVG_SIZE] = {0};
	    static uint8_t  adc_index = 0;
        static uint8_t  adc_filled = 0;

	    adc_buffer[adc_index] = adc_value;
	    adc_index = (adc_index + 1) % MOVING_AVG_SIZE;

        // Remplissage initial avec la première valeur
        if (!adc_filled) {
            for (uint8_t i = 0; i < MOVING_AVG_SIZE; i++)
                adc_buffer[i] = adc_value;
            adc_filled = 1;
        }

	    uint32_t sum = 0;
	    for (uint8_t i = 0; i < MOVING_AVG_SIZE; i++) sum += adc_buffer[i];
	    uint16_t avg = sum / MOVING_AVG_SIZE;

	    uint8_t is_active = (avg >= MORSE_ADC_THRESHOLD);

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
                    rcv->last_flush_ms  = now_ms;
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
                    rcv->last_flush_ms  = now_ms;
                    rcv->debounce_count = 0;
                }
            } else {
                rcv->debounce_count = 0;
            }
            break;
    }

    /* Flush de la dernière lettre après un long silence */
    if (rcv->state == RECEIVER_SILENT)
    {
        uint32_t total_silence  = now_ms - rcv->timestamp_ms;
        uint32_t since_flush    = now_ms - rcv->last_flush_ms;

        /* 1) Fin de message : silence très long → marquer le message prêt */
        if (total_silence >= MORSE_END_OF_MSG_TIMEOUT_MS)
        {
            if (!rcv->decoder.message_ready && rcv->decoder.message_len > 0)
            {
                morse_decoder_flush(&rcv->decoder, &rcv->table);
                rcv->decoder.message_ready = 1;
            }
        }
        /* 2) Flush intermédiaire : valider la lettre en cours si trop long silence */
        else if (since_flush >= MORSE_FLUSH_TIMEOUT_MS)
        {
            morse_decoder_flush(&rcv->decoder, &rcv->table);
            rcv->last_flush_ms = now_ms;   /* seulement last_flush_ms est mis à jour */
            /* timestamp_ms reste inchangé → le silence total continue de s'accumuler */
        }
    }
}

uint8_t morse_receiver_is_done(const MorseReceiver *rcv)
{
    if (rcv == NULL) return 0;
    return rcv->decoder.message_ready;
}

const char *morse_receiver_get_message(const MorseReceiver *rcv)
{
    if (rcv == NULL) return NULL;
    return morse_decoder_get_message(&rcv->decoder);
}
