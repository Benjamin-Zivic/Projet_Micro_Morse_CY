#ifndef MORSE_TIMING_H
#define MORSE_TIMING_H

#include <stdint.h>

/* Catégories possibles pour une durée mesurée (signal actif ou silence) */
typedef enum {
    MORSE_TIMING_DOT =  0,  /* Signal court → point  */
    MORSE_TIMING_DASH =  1,  /* Signal long → trait */
    MORSE_TIMING_INTRA =  2,  /* Silence court → entre deux symboles d'une même lettre */
    MORSE_TIMING_INTER_CHAR =  3,  /* Silence moyen → entre deux lettres */
    MORSE_TIMING_INTER_WORD =  4,  /* Silence long → entre deux mots */
    MORSE_TIMING_UNKNOWN = -1   /* Durée hors des plages acceptées */
} MorseTimingClass;

/**
 * Analyse une durée mesurée et retourne la catégorie correspondante.
 * is_active indique si la durée correspond à un signal sonore (1) ou à un silence (0).
 */
MorseTimingClass morse_classify_duration(uint32_t duration_ms, uint8_t is_active);

#endif