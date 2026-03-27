#ifndef MORSE_RECEIVER_H
#define MORSE_RECEIVER_H

#include "morse.h"
#include "morse_timing.h"
#include "morse_decoder.h"
#include <stdint.h>

/* États possibles du récepteur */
typedef enum {
    RECEIVER_IDLE,    /* Aucun signal reçu, en attente d'activité */
    RECEIVER_ACTIVE,  /* Un signal sonore est en cours */
    RECEIVER_SILENT,  /* Un silence est en cours */
} ReceiverState;

/* Structure principale du récepteur */
typedef struct {
    ReceiverState state;
    uint32_t timestamp_ms;   /* Horodatage du début de l'état courant (signal ou silence) */
    uint32_t last_flush_ms;  /* Horodatage du dernier flush intermédiaire */
    uint8_t debounce_count;  /* Compteur anti-rebond : nombre d'échantillons consécutifs confirmant l'état */
    MorseDecoder decoder;    /* Décodeur morse associé */
    MorseTable table;        /* Table de correspondance morse chargée en mémoire */
} MorseReceiver;

/* Initialise le récepteur et charge la table morse */
MorseStatus morse_receiver_init(MorseReceiver *rcv);

/* Appeler périodiquement (depuis l'interruption TIM2) avec la valeur ADCet l'horodatage courant */
void morse_receiver_update(MorseReceiver *rcv, uint16_t adc_value, uint32_t now_ms);

/* Retourne 1 si le message complet a été reçu et décodé, 0 sinon */
uint8_t morse_receiver_is_done(const MorseReceiver *rcv);

/* Retourne le message décodé sous forme de chaîne C */
const char *morse_receiver_get_message(const MorseReceiver *rcv);

#endif