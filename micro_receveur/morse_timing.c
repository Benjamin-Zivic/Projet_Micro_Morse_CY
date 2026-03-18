#include "morse_timing.h"
#include "config.h"

/* =========================================================
 *  Macro interne de tolérance
 *  Vérifie si 'val' est dans [target - x% ; target + x%]
 * ========================================================= */

#define IN_RANGE(val, target) \
    ((val) >= (uint32_t)(target) * (100 - MORSE_TOLERANCE_PCT) / 100 && \
     (val) <= (uint32_t)(target) * (100 + MORSE_TOLERANCE_PCT) / 100)

/* =========================================================
 *  Implémentation
 * ========================================================= */

MorseTimingClass morse_classify_duration(uint32_t duration_ms,
                                         uint8_t  is_active)
{
    if (is_active) {
        /* --- Signal sonore : point ou trait --- */
        if (IN_RANGE(duration_ms, MORSE_DOT_MS))  return MORSE_TIMING_DOT;
        if (IN_RANGE(duration_ms, MORSE_DASH_MS)) return MORSE_TIMING_DASH;
    } else {
        /* --- Silence : trois niveaux de séparation --- */
        if (IN_RANGE(duration_ms, MORSE_INTER_WORD_MS)) return MORSE_TIMING_INTER_WORD;
        if (IN_RANGE(duration_ms, MORSE_INTER_CHAR_MS)) return MORSE_TIMING_INTER_CHAR;
        if (IN_RANGE(duration_ms, MORSE_INTRA_CHAR_MS)) return MORSE_TIMING_INTRA;
    }

    return MORSE_TIMING_UNKNOWN;
}