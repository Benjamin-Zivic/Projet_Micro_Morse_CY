#ifndef CONFIG_H
#define CONFIG_H

/* Durée de base du morse en millisecondes */
#define MORSE_UNIT_MS  60

/* Durées calculées à partir de l'unité de base */
#define MORSE_DOT_MS  (1 * MORSE_UNIT_MS)   /* Point  :  60 ms */
#define MORSE_DASH_MS  (3 * MORSE_UNIT_MS)   /* Trait  : 180 ms */
#define MORSE_INTRA_CHAR_MS  (1 * MORSE_UNIT_MS)   /* Silence entre deux éléments d'une même lettre :  60 ms */
#define MORSE_INTER_CHAR_MS  (3 * MORSE_UNIT_MS)   /* Silence entre deux lettres : 180 ms */
#define MORSE_INTER_WORD_MS  (7 * MORSE_UNIT_MS)   /* Silence entre deux mots    : 420 ms */

/* Taille maximale du message reçu (en caractères) */
#define MORSE_MSG_MAX_LEN  128

/* Délai avant de forcer l'écriture du dernier élément en attente */
#define MORSE_FLUSH_TIMEOUT_MS  500

/* Durée de silence qui indique la fin du message complet */
#define MORSE_END_OF_MSG_TIMEOUT_MS  1500

/* Tolérance en % pour la reconnaissance des durées.
   Ex : avec 30%, un point attendu à 60 ms est accepté entre 42 ms et 78 ms */
#define MORSE_TOLERANCE_PCT  30

/* Valeur ADC au-dessus de laquelle le signal est considéré comme actif */
#define MORSE_ADC_THRESHOLD  500

/* Nombre d'échantillons consécutifs nécessaires pour valider un changement d'état */
#define MORSE_DEBOUNCE_SAMPLES  8

/* Fréquence à laquelle morse_receiver_update() est appelée (en Hz) */
#define MORSE_SAMPLE_RATE_HZ  5000

/* Période entre deux appels en millisecondes */
#define MORSE_SAMPLE_PERIOD_MS  (1000 / MORSE_SAMPLE_RATE_HZ)

/* Paramètres du timer TIM2 pour générer une interruption à la fréquence MORSE_SAMPLE_RATE_HZ */
#define TIM2_PERIOD  999
#define TIM2_PRESCALER  ((84000000 / (MORSE_SAMPLE_RATE_HZ * (TIM2_PERIOD + 1))) - 1)

#endif