#include "morse_decoder.h"
#include "config.h"
#include <string.h>

static void reset_current_code(MorseDecoder *dec)
{
    dec->current_code.length = 0;
}

void morse_decoder_init(MorseDecoder *dec)
{
    if (dec == NULL) return;

    dec->state       = DECODER_WAITING_START;
    dec->message_len = 0;
    memset(dec->message, 0, MORSE_MSG_MAX_LEN);
    reset_current_code(dec);
}

/* Vérifie si current_code correspond au caractère attendu */
static uint8_t code_matches_char(const MorseTable *table,
                                 const MorseCode  *code,
                                 char              c)
{
    MorseCode expected;
    if (morse_lookup_char(table, c, &expected) != MORSE_OK) return 0;

    if (code->length != expected.length) return 0;
    for (uint8_t i = 0; i < code->length; i++) {
        if (code->symbols[i] != expected.symbols[i]) return 0;
    }
    return 1;
}

static void validate_letter(MorseDecoder     *dec,
                             const MorseTable *table)
{
    if (dec->current_code.length == 0) return;

    if (dec->state == DECODER_WAITING_START) {

        /* On cherche KA = 'K' puis 'A' sur deux lettres consécutives */
        static uint8_t ka_k_seen = 0;

        if (!ka_k_seen && code_matches_char(table, &dec->current_code, 'K')) {
            ka_k_seen = 1;
        } else if (ka_k_seen && code_matches_char(table, &dec->current_code, 'A')) {
            ka_k_seen = 0;
            dec->state = DECODER_RECEIVING;
        } else {
            ka_k_seen = 0;  /* séquence interrompue, on repart */
        }

    } else if (dec->state == DECODER_RECEIVING) {

        /* On cherche AR = 'A' puis 'R' sur deux lettres consécutives */
        static uint8_t ar_a_seen = 0;

        if (!ar_a_seen && code_matches_char(table, &dec->current_code, 'A')) {
            ar_a_seen = 1;
        } else if (ar_a_seen && code_matches_char(table, &dec->current_code, 'R')) {
            ar_a_seen = 0;
            dec->state = DECODER_DONE;
        } else {
            /* Pas un prosigne AR : on décode normalement */
            if (ar_a_seen) {
                /* Le 'A' précédent était une vraie lettre, on l'ajoute */
                if (dec->message_len < MORSE_MSG_MAX_LEN - 1)
                    dec->message[dec->message_len++] = 'A';
                ar_a_seen = 0;
            }

            char out = '?';
            morse_decode(table, &dec->current_code, &out);
            if (dec->message_len < MORSE_MSG_MAX_LEN - 1)
                dec->message[dec->message_len++] = out;
        }
    }

    reset_current_code(dec);
}

void morse_decoder_push(MorseDecoder     *dec,
                        const MorseTable *table,
                        MorseTimingClass  cls)
{
    if (dec == NULL || table == NULL) return;
    if (dec->state == DECODER_DONE)  return;

    switch (cls) {

        case MORSE_TIMING_DOT:
            if (dec->current_code.length < MORSE_MAX_SYMBOLS)
                dec->current_code.symbols[dec->current_code.length++] = MORSE_DOT;
            break;

        case MORSE_TIMING_DASH:
            if (dec->current_code.length < MORSE_MAX_SYMBOLS)
                dec->current_code.symbols[dec->current_code.length++] = MORSE_DASH;
            break;

        case MORSE_TIMING_INTRA:
            /* séparation inter-éléments : rien à faire */
            break;

        case MORSE_TIMING_INTER_CHAR:
            validate_letter(dec, table);
            break;

        case MORSE_TIMING_INTER_WORD:
            validate_letter(dec, table);
            if (dec->state == DECODER_RECEIVING &&
                dec->message_len < MORSE_MSG_MAX_LEN - 1)
                dec->message[dec->message_len++] = ' ';
            break;

        case MORSE_TIMING_UNKNOWN:
        default:
            break;
    }
}

const char *morse_decoder_get_message(const MorseDecoder *dec)
{
    if (dec == NULL) return NULL;
    return dec->message;
}