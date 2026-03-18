#ifndef MORSE_DECODER_H
#define MORSE_DECODER_H

#include "morse.h"
#include "morse_timing.h"
#include <stdint.h>

#define MORSE_MSG_MAX_LEN  128

typedef enum {
    DECODER_WAITING_START,
    DECODER_RECEIVING,
    DECODER_DONE
} DecoderState;

typedef struct {
    DecoderState state;
    MorseCode    current_code;
    char         message[MORSE_MSG_MAX_LEN];
    uint8_t      message_len;
} MorseDecoder;

/**
 * @brief Initialise le décodeur.
 */
void morse_decoder_init(MorseDecoder *dec);

/**
 * @brief Soumet une classe temporelle au décodeur.
 *
 * À appeler à chaque fois qu'un signal ou silence est mesuré.
 *
 * @param dec    Décodeur initialisé.
 * @param table  Table morse (pour la résolution caractère).
 * @param cls    Classe temporelle issue de morse_classify_duration().
 */
void morse_decoder_push(MorseDecoder        *dec,
                        const MorseTable    *table,
                        MorseTimingClass     cls);

/**
 * @brief Retourne le message décodé (valide si state == DECODER_DONE).
 */
const char *morse_decoder_get_message(const MorseDecoder *dec);

#endif /* MORSE_DECODER_H */