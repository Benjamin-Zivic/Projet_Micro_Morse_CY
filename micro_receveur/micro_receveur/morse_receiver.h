#ifndef MORSE_RECEIVER_H
#define MORSE_RECEIVER_H

#include "morse.h"
#include "morse_timing.h"
#include "morse_decoder.h"
#include <stdint.h>

typedef enum {
    RECEIVER_IDLE,      /* en attente du prosigne KA   */
    RECEIVER_ACTIVE,    /* signal sonore en cours       */
    RECEIVER_SILENT,    /* silence en cours             */
} ReceiverState;

typedef struct {
    ReceiverState  state;
    uint32_t       timestamp_ms;    /* début de l'état courant      */
    uint8_t        debounce_count;  /* compteur anti-rebond         */
    MorseDecoder   decoder;
    MorseTable     table;
} MorseReceiver;

/**
 * @brief Initialise le récepteur et charge la table morse.
 */
MorseStatus morse_receiver_init(MorseReceiver *rcv);

/**
 * @brief À appeler périodiquement avec la valeur ADC courante
 *        et le timestamp courant (issu de HAL_GetTick()).
 *
 * @param rcv         Récepteur initialisé.
 * @param adc_value   Valeur ADC brute (0–4095).
 * @param now_ms      Timestamp courant en ms (HAL_GetTick()).
 */
void morse_receiver_update(MorseReceiver *rcv,
                           uint16_t       adc_value,
                           uint32_t       now_ms);

/**
 * @brief Retourne 1 si le message complet a été reçu.
 */
uint8_t morse_receiver_is_done(const MorseReceiver *rcv);

/**
 * @brief Retourne le message décodé (valide si is_done == 1).
 */
const char *morse_receiver_get_message(const MorseReceiver *rcv);

#endif /* MORSE_RECEIVER_H */
