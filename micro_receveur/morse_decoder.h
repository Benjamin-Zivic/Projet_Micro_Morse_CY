#ifndef MORSE_DECODER_H
#define MORSE_DECODER_H

#include "morse.h"
#include "morse_timing.h"
#include "config.h"
#include <stdint.h>

/* États possibles du décodeur */
typedef enum {
    DECODER_WAITING_START,  
    DECODER_RECEIVING,      /* Réception et décodage en cours           */
    DECODER_DONE            /* Message complet reçu, prêt à être lu     */
} DecoderState;

/* Structure principale du décodeur morse */
typedef struct {
    DecoderState state;
    MorseCode current_code;              /* Code morse de la lettre en cours de construction */
    char message[MORSE_MSG_MAX_LEN];     /* Message décodé accumulé */
    uint8_t message_len;                 /* Nombre de caractères décodés jusqu'ici */
    uint8_t message_ready;               /* Mis à 1 quand le message est complet */
} MorseDecoder;

/* Initialise le décodeur et remet tous ses champs à zéro */
void morse_decoder_init(MorseDecoder *dec);

/**
 * Soumet une classe temporelle au décodeur.
 * À appeler à chaque fois qu'un signal actif ou un silence est mesuré.
 */
void morse_decoder_push(MorseDecoder *dec, const MorseTable *table, MorseTimingClass cls);

/* Retourne le message décodé sous forme de chaîne */
const char *morse_decoder_get_message(const MorseDecoder *dec);

/* Force la validation de la lettre en cours de construction */
void morse_decoder_flush(MorseDecoder *dec, const MorseTable *table);

#endif 