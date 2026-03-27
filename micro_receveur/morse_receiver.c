#include "morse_receiver.h"
#include "config.h"
#include <stddef.h>

/* Taille du tampon pour le calcul de la moyenne glissante */
#define MOVING_AVG_SIZE  8

/* Initialise tous les champs du récepteur et charge la table morse */
MorseStatus morse_receiver_init(MorseReceiver *rcv)
{
    if (rcv == NULL) return MORSE_ERR_NULL;

    rcv->state = RECEIVER_IDLE;
    rcv->timestamp_ms = 0;
    rcv->last_flush_ms = 0;
    rcv->debounce_count = 0;

    morse_decoder_init(&rcv->decoder);
    return morse_table_load(&rcv->table);
}

/* Traite un nouvel échantillon ADC et met à jour l'état du récepteur */
void morse_receiver_update(MorseReceiver *rcv, uint16_t adc_value, uint32_t now_ms)
{
    if (rcv == NULL) return;
    if (morse_receiver_is_done(rcv)) return;  /* Ne rien faire si le message est déjà complet */

    /* --- Moyenne glissante sur les 8 derniers échantillons ---
       Réduit l'effet du bruit ponctuel sur la détection du signal.
       Les variables sont statiques pour conserver leur valeur entre les appels. */
    static uint16_t adc_buffer[MOVING_AVG_SIZE] = {0};
    static uint8_t  adc_index = 0;
    static uint8_t  adc_filled = 0;

    adc_buffer[adc_index] = adc_value;
    adc_index = (adc_index + 1) % MOVING_AVG_SIZE;

    /* Premier appel : on remplit tout le tampon avec la valeur initiale
       pour éviter une moyenne faussée par des zéros */
    if (!adc_filled) {
        for (uint8_t i = 0; i < MOVING_AVG_SIZE; i++)
            adc_buffer[i] = adc_value;
        adc_filled = 1;
    }

    /* Calcul de la moyenne des 8 derniers échantillons */
    uint32_t sum = 0;
    for (uint8_t i = 0; i < MOVING_AVG_SIZE; i++) sum += adc_buffer[i];
    uint16_t avg = sum / MOVING_AVG_SIZE;

    /* Le signal est considéré actif si la moyenne dépasse le seuil défini dans config.h */
    uint8_t is_active = (avg >= MORSE_ADC_THRESHOLD);

    switch (rcv->state) {

        case RECEIVER_IDLE:
        case RECEIVER_SILENT:
            if (is_active) {
                /* Signal détecté : on incrémente le compteur anti-rebond */
                rcv->debounce_count++;
                if (rcv->debounce_count >= MORSE_DEBOUNCE_SAMPLES) {
                    /* Transition silence → signal confirmée après N échantillons consécutifs */
                    uint32_t silence_duration = now_ms - rcv->timestamp_ms;
                    MorseTimingClass cls =
                        morse_classify_duration(silence_duration, 0); /* 0 = silence */
                    morse_decoder_push(&rcv->decoder, &rcv->table, cls);

                    rcv->state = RECEIVER_ACTIVE;
                    rcv->timestamp_ms = now_ms;  /* On note le début du signal */
                    rcv->last_flush_ms = now_ms;
                    rcv->debounce_count = 0;
                }
            } else {
                rcv->debounce_count = 0;  /* Signal instable : on repart de zéro */
            }
            break;

        case RECEIVER_ACTIVE:
            if (!is_active) {
                /* Silence détecté : on incrémente le compteur anti-rebond */
                rcv->debounce_count++;
                if (rcv->debounce_count >= MORSE_DEBOUNCE_SAMPLES) {
                    /* Transition signal → silence confirmée après N échantillons consécutifs */
                    uint32_t signal_duration = now_ms - rcv->timestamp_ms;
                    MorseTimingClass cls =
                        morse_classify_duration(signal_duration, 1); /* 1 = signal actif */
                    morse_decoder_push(&rcv->decoder, &rcv->table, cls);

                    rcv->state = RECEIVER_SILENT;
                    rcv->timestamp_ms = now_ms;  /* On note le début du silence */
                    rcv->last_flush_ms = now_ms;
                    rcv->debounce_count = 0;
                }
            } else {
                rcv->debounce_count = 0;  /* Signal encore présent : on repart de zéro */
            }
            break;
    }

    /* Permet de valider la dernière lettre ou de clore le message
       sans attendre un prochain signal qui pourrait ne jamais arriver. */
    if (rcv->state == RECEIVER_SILENT)
    {
        uint32_t total_silence = now_ms - rcv->timestamp_ms;
        uint32_t since_flush = now_ms - rcv->last_flush_ms;

        /* Cas 1 : silence très long → fin de message */
        if (total_silence >= MORSE_END_OF_MSG_TIMEOUT_MS)
        {
            if (!rcv->decoder.message_ready && rcv->decoder.message_len > 0)
            {
                morse_decoder_flush(&rcv->decoder, &rcv->table);
                rcv->decoder.message_ready = 1;
            }
        }
        /* Cas 2 : silence intermédiaire assez long → on valide la lettre en cours
           sans pour autant clore le message */
        else if (since_flush >= MORSE_FLUSH_TIMEOUT_MS)
        {
            morse_decoder_flush(&rcv->decoder, &rcv->table);
            rcv->last_flush_ms = now_ms;  /* On remet uniquement last_flush_ms à jour */
        }
    }
}

/* Retourne 1 si le message complet a été reçu, 0 sinon */
uint8_t morse_receiver_is_done(const MorseReceiver *rcv)
{
    if (rcv == NULL) return 0;
    return rcv->decoder.message_ready;
}

/* Retourne le message décodé (uniquement valide si morse_receiver_is_done() == 1) */
const char *morse_receiver_get_message(const MorseReceiver *rcv)
{
    if (rcv == NULL) return NULL;
    return morse_decoder_get_message(&rcv->decoder);
}