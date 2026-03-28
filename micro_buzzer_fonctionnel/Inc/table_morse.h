#ifndef TABLE_MORSE_H
#define TABLE_MORSE_H

#include <stdint.h>

/* Nombre maximum de symboles (points/traits) dans un code morse */
#define MORSE_MAX_SYMBOLS  8
/* Nombre total d'entrées dans la table : 26 lettres + 10 chiffres + ponctuation */
#define MORSE_TABLE_SIZE  54

/* Les deux symboles élémentaires du morse */
typedef enum {
    MORSE_DOT = 0,   /* Point (signal court) */
    MORSE_DASH = 1    /* Trait (signal long) */
} MorseSymbol;

/* Séquence de symboles représentant un caractère en morse */
typedef struct {
    MorseSymbol symbols[MORSE_MAX_SYMBOLS];
    uint8_t length;   /* Nombre de symboles valides dans le tableau */
} MorseCode;

/* Association entre un caractère et son code morse */
typedef struct {
    char character;
    MorseCode code;
} MorseEntry;

/* Table de correspondance complète, stockée en mémoire Flash */
typedef struct {
    const MorseEntry *entries;
    uint8_t count;    /* Nombre d'entrées dans la table */
} MorseTable;


typedef enum {
    MORSE_OK = 0,
    MORSE_ERR_NULL = -1,       /* Un pointeur NULL a été passé en argument */
    MORSE_ERR_NOTFOUND = -2,   /* Le caractère n'existe pas dans la table */
} MorseStatus;

/**
 * Initialise la table morse en la faisant pointer vers les données constantes en Flash.
 * À appeler une fois avant toute utilisation de la table.
 */
MorseStatus morse_table_load(MorseTable *table);

/**
 * Cherche le code morse d'un caractère donné.
 * Exemple : morse_lookup_char(table, 'A', &code) remplit code avec {DOT, DASH}.
 */
MorseStatus morse_lookup_char(const MorseTable *table, char c, MorseCode *out);

/**
 * Fait l'opération inverse : à partir d'un code morse, retrouve le caractère.
 */
MorseStatus morse_decode(const MorseTable *table, const MorseCode *code, char *out);

#endif