#include "morse_timing.h"
#include "config.h"

/* Vérifie si la valeur est dans la plage [target - TOLERANCE% ; target + TOLERANCE%] */
#define IN_RANGE(val, target) \
    ((val) >= (uint32_t)(target) * (100 - MORSE_TOLERANCE_PCT) / 100 && \
     (val) <= (uint32_t)(target) * (100 + MORSE_TOLERANCE_PCT) / 100)


MorseTimingClass morse_classify_duration(uint32_t duration_ms, uint8_t is_active)
{
    if (is_active) {
        /* Signal sonore : on détermine si c'est un point ou un trait */
        if (IN_RANGE(duration_ms, MORSE_DOT_MS))  return MORSE_TIMING_DOT;
        if (IN_RANGE(duration_ms, MORSE_DASH_MS)) return MORSE_TIMING_DASH;
    } else {
        /* Silence : on distingue les trois niveaux de séparation */
        
        uint32_t inter_char_max = (uint32_t)MORSE_INTER_CHAR_MS * (100 + MORSE_TOLERANCE_PCT) / 100;

        if (duration_ms > inter_char_max) return MORSE_TIMING_INTER_WORD;
        if (IN_RANGE(duration_ms, MORSE_INTER_CHAR_MS)) return MORSE_TIMING_INTER_CHAR;
        if (IN_RANGE(duration_ms, MORSE_INTRA_CHAR_MS)) return MORSE_TIMING_INTRA;
    }

    return MORSE_TIMING_UNKNOWN;  /* Durée non reconnue */
}