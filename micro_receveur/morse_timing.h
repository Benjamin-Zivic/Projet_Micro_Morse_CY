#ifndef MORSE_TIMING_H
#define MORSE_TIMING_H

#include <stdint.h>

typedef enum {
    MORSE_TIMING_DOT        =  0,  /* signal actif court  : point          */
    MORSE_TIMING_DASH       =  1,  /* signal actif long   : trait          */
    MORSE_TIMING_INTRA      =  2,  /* silence court       : inter-éléments */
    MORSE_TIMING_INTER_CHAR =  3,  /* silence moyen       : inter-lettres  */
    MORSE_TIMING_INTER_WORD =  4,  /* silence long        : inter-mots     */
    MORSE_TIMING_UNKNOWN    = -1   /* hors tolérance                       */
} MorseTimingClass;

/**
 * @brief Classifie une durée mesurée en ms.
 *
 * @param duration_ms  Durée mesurée (signal actif ou silence).
 * @param is_active    1 si signal actif (son), 0 si silence.
 * @return             La classe temporelle correspondante.
 */
MorseTimingClass morse_classify_duration(uint32_t duration_ms,
                                         uint8_t  is_active);

#endif