#ifndef CONFIG_H
#define CONFIG_H

/* =========================================================
 *  Unité de temps de base du morse (en millisecondes)
 *  À 20 WPM standard : 60 ms
 * ========================================================= */
#define MORSE_UNIT_MS           60

/* =========================================================
 *  Durées dérivées (calculées automatiquement)
 * ========================================================= */
#define MORSE_DOT_MS            (1 * MORSE_UNIT_MS)   /*  60 ms */
#define MORSE_DASH_MS           (3 * MORSE_UNIT_MS)   /* 180 ms */
#define MORSE_INTRA_CHAR_MS     (1 * MORSE_UNIT_MS)   /*  60 ms — entre éléments d'une même lettre */
#define MORSE_INTER_CHAR_MS     (3 * MORSE_UNIT_MS)   /* 180 ms — entre deux lettres */
#define MORSE_INTER_WORD_MS     (7 * MORSE_UNIT_MS)   /* 420 ms — entre deux mots */

/* =========================================================
 *  Marges de tolérance pour le décodeur (en %)
 *  Un signal mesuré est accepté si sa durée est dans
 *  [valeur * (100 - TOLERANCE) / 100 ;
 *   valeur * (100 + TOLERANCE) / 100]
 * ========================================================= */
#define MORSE_TOLERANCE_PCT     30

/* Seuil ADC au-dessus duquel on considère un signal actif (0-4095) */
#define MORSE_ADC_THRESHOLD   500
/* Nombre d'échantillons consécutifs pour confirmer une transition */
/* Évite les faux déclenchements sur bruit bref                    */
#define MORSE_DEBOUNCE_SAMPLES 8 //8

/* Fréquence d'appel de morse_receiver_update() en Hz */
#define MORSE_SAMPLE_RATE_HZ     5000

/* Période correspondante en ms */
#define MORSE_SAMPLE_PERIOD_MS   (1000 / MORSE_SAMPLE_RATE_HZ)  /* 10 ms */

#define TIM2_PERIOD      999
#define TIM2_PRESCALER   ((84000000 / (MORSE_SAMPLE_RATE_HZ * (TIM2_PERIOD + 1))) - 1)

/* =========================================================
 *  Séquences de début et fin de transmission
 * ========================================================= */
#define MORSE_SEQ_START         "KA"   /* -.-.- : début de message */
#define MORSE_SEQ_END           "AR"   /* .-.-. : fin de message   */

#endif /* CONFIG_H */
