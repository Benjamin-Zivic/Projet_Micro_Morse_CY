#include "morse.h"
#include <stddef.h>
#include <ctype.h>

/* =========================================================
 *  Table caractères en Flash (.rodata)
 * ========================================================= */

static const MorseEntry MORSE_TABLE_DATA[] = {

    /* --- Lettres --- */
    { 'A', { {MORSE_DOT,  MORSE_DASH},                                                              2 } },
    { 'B', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                                      4 } },
    { 'C', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT},                                      4 } },
    { 'D', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT},                                                  3 } },
    { 'E', { {MORSE_DOT},                                                                           1 } },
    { 'F', { {MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DOT},                                      4 } },
    { 'G', { {MORSE_DASH, MORSE_DASH, MORSE_DOT},                                                  3 } },
    { 'H', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                                      4 } },
    { 'I', { {MORSE_DOT,  MORSE_DOT},                                                              2 } },
    { 'J', { {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DASH},                                     4 } },
    { 'K', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH},                                                 3 } },
    { 'L', { {MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DOT},                                      4 } },
    { 'M', { {MORSE_DASH, MORSE_DASH},                                                             2 } },
    { 'N', { {MORSE_DASH, MORSE_DOT},                                                              2 } },
    { 'O', { {MORSE_DASH, MORSE_DASH, MORSE_DASH},                                                 3 } },
    { 'P', { {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT},                                      4 } },
    { 'Q', { {MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DASH},                                     4 } },
    { 'R', { {MORSE_DOT,  MORSE_DASH, MORSE_DOT},                                                  3 } },
    { 'S', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                                                  3 } },
    { 'T', { {MORSE_DASH},                                                                          1 } },
    { 'U', { {MORSE_DOT,  MORSE_DOT,  MORSE_DASH},                                                 3 } },
    { 'V', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH},                                     4 } },
    { 'W', { {MORSE_DOT,  MORSE_DASH, MORSE_DASH},                                                 3 } },
    { 'X', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DASH},                                     4 } },
    { 'Y', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DASH},                                     4 } },
    { 'Z', { {MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT},                                      4 } },

    /* --- Chiffres --- */
    { '0', { {MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DASH},                         5 } },
    { '1', { {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DASH},                         5 } },
    { '2', { {MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DASH},                         5 } },
    { '3', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DASH},                         5 } },
    { '4', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH},                         5 } },
    { '5', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                          5 } },
    { '6', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                          5 } },
    { '7', { {MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                          5 } },
    { '8', { {MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT},                          5 } },
    { '9', { {MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DOT},                          5 } },

    /* --- Ponctuation --- */
    { '.', { {MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DASH},             6 } },
    { ',', { {MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DASH},             6 } },
    { '?', { {MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT},              6 } },
    { '!', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DASH},             6 } },
    { '\'',{ {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DOT},              6 } },
    { '"', { {MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DOT},              6 } },
    { '(', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT},                          5 } },
    { ')', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DASH},             6 } },
    { '&', { {MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT},                          5 } },
    { ':', { {MORSE_DASH, MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT},              6 } },
    { ';', { {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT},              6 } },
    { '/', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DOT},                          5 } },
    { '-', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH},             6 } },
    { '_', { {MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DASH},             6 } },
    { '$', { {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DASH}, 7 } },
    { '@', { {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT},              6 } },
    { '=', { {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH},                         5 } },
    { '+', { {MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT},                          5 } },
};

#define MORSE_TABLE_COUNT (sizeof(MORSE_TABLE_DATA) / sizeof(MORSE_TABLE_DATA[0]))

/* =========================================================
 *  Fonction interne
 * ========================================================= */

static int morse_code_equal(const MorseCode *a, const MorseCode *b)
{
    if (a->length != b->length) return 0;
    for (uint8_t i = 0; i < a->length; i++) {
        if (a->symbols[i] != b->symbols[i]) return 0;
    }
    return 1;
}

/* =========================================================
 *  Implémentation de l'API
 * ========================================================= */

MorseStatus morse_table_load(MorseTable *table)
{
    if (table == NULL) return MORSE_ERR_NULL;

    table->entries = MORSE_TABLE_DATA;
    table->count   = (uint8_t)MORSE_TABLE_COUNT;

    return MORSE_OK;
}

/* --------------------------------------------------------- */

MorseStatus morse_lookup_char(const MorseTable *table,
                              char              c,
                              MorseCode        *out)
{
    if (table == NULL || out == NULL) return MORSE_ERR_NULL;

    char upper = (char)toupper((unsigned char)c);

    for (uint8_t i = 0; i < table->count; i++) {
        if (table->entries[i].character == upper) {
            *out = table->entries[i].code;
            return MORSE_OK;
        }
    }
    return MORSE_ERR_NOTFOUND;
}

/* --------------------------------------------------------- */

MorseStatus morse_decode(const MorseTable *table,
                         const MorseCode  *code,
                         char             *out)
{
    if (table == NULL || code == NULL || out == NULL) return MORSE_ERR_NULL;

    for (uint8_t i = 0; i < table->count; i++) {
        if (morse_code_equal(&table->entries[i].code, code)) {
            *out = table->entries[i].character;
            return MORSE_OK;
        }
    }
    return MORSE_ERR_NOTFOUND;
}
