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

    dec->state       = DECODER_RECEIVING;
    dec->message_len = 0;
    dec->message_ready = 0;
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

    char out = '?';
    morse_decode(table, &dec->current_code, &out);
    if (dec->message_len < MORSE_MSG_MAX_LEN - 1)
        dec->message[dec->message_len++] = out;

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

void morse_decoder_flush(MorseDecoder *dec, const MorseTable *table)
{
    if (dec == NULL || table == NULL) return;
    if (dec->current_code.length > 0)
        validate_letter(dec, table);
}
