#ifndef TABLE_MORSE_H
#define TABLE_MORSE_H

#include <stdint.h>

/* =========================================================
 *  Constantes
 * ========================================================= */

#define MORSE_MAX_SYMBOLS  8   /* max de . / - par caractère            */
#define MORSE_TABLE_SIZE  54   /* 26 lettres + 10 chiffres + ponct      */

/* =========================================================
 *  Types
 * ========================================================= */

/**
 * @brief Symbole élémentaire morse : point ou trait.
 */
typedef enum {
    MORSE_DOT  = 0,
    MORSE_DASH = 1
} MorseSymbol;

/**
 * @brief Représentation morse d'un seul caractère.
 */
typedef struct {
    MorseSymbol symbols[MORSE_MAX_SYMBOLS];
    uint8_t     length;   /* nombre de symboles valides                 */
} MorseCode;

/**
 * @brief Entrée caractère → code morse.
 */
typedef struct {
    char      character;
    MorseCode code;
} MorseEntry;

/**
 * @brief Table de lookup complète (pointe vers des données const en Flash).
 */
typedef struct {
    const MorseEntry *entries;
    uint8_t           count;
} MorseTable;

/* =========================================================
 *  Codes de retour
 * ========================================================= */

typedef enum {
    MORSE_OK           =  0,
    MORSE_ERR_NULL     = -1,   /* pointeur NULL passé en argument       */
    MORSE_ERR_NOTFOUND = -2,   /* caractère absent de la table          */
} MorseStatus;

/* =========================================================
 *  API publique
 * ========================================================= */

/**
 * @brief Initialise la table morse depuis les données const en Flash.
 *
 * @param table  Pointeur vers la MorseTable à initialiser.
 * @return MORSE_OK si succès, MORSE_ERR_NULL si pointeur invalide.
 */
MorseStatus morse_table_load(MorseTable *table);

/**
 * @brief Recherche le MorseCode d'un caractère donné.
 *
 * @param table  Table morse initialisée.
 * @param c      Caractère à chercher (insensible à la casse).
 * @param out    MorseCode correspondant.
 * @return MORSE_OK si trouvé, MORSE_ERR_NOTFOUND sinon.
 */
MorseStatus morse_lookup_char(const MorseTable *table,
                              char              c,
                              MorseCode        *out);

/**
 * @brief Décode un MorseCode unique en caractère.
 *
 * @param table  Table morse initialisée.
 * @param code   Code morse à décoder.
 * @param out    Caractère résultant (majuscule).
 * @return MORSE_OK si trouvé, MORSE_ERR_NOTFOUND sinon.
 */
MorseStatus morse_decode(const MorseTable *table,
                         const MorseCode  *code,
                         char             *out);

#endif /* MORSE_H */
