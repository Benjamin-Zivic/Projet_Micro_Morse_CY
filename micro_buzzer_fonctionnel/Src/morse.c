#include "morse.h"
#include "table_morse.h"
#include "buzzer.h"
#include <string.h>

static MorseTable table;            /* Table de correspondance morse, chargée au démarrage */
volatile uint8_t buzzer_actif = 0;  /* Drapeau : 1 pendant qu'une émission est en cours */

/* Charge la table morse en mémoire. */
void morse_init(void) {
    morse_table_load(&table);
}

/* Convertit un texte en chaîne morse
   Les espaces du texte deviennent '/' dans la chaîne morse.
   Les lettres sont séparées par un espace dans la chaîne de sortie. */
void text_to_morse(const char *text, char *output, uint16_t output_size) {
    output[0] = '\0';

    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        if (c == ' ') {
            strncat(output, "/", output_size - strlen(output) - 1);
            continue;
        }

        MorseCode code;
        if (morse_lookup_char(&table, c, &code) == MORSE_OK) {
            for (uint8_t j = 0; j < code.length; j++) {
                char sym[2] = { code.symbols[j] == MORSE_DOT ? '.' : '-', '\0' };
                strncat(output, sym, output_size - strlen(output) - 1);
            }
            /* Séparateur entre les lettres */
            strncat(output, " ", output_size - strlen(output) - 1);
        }
    }
}

/* Joue une chaîne morse sur le buzzer en respectant les durées standard */
void buzzer_morse(const char *morse) {
    //buzzer_actif = 1;
    buzzer_set_freq(1000);      /* Fréquence du son : 1000 Hz */
    buzzer_set_volume(100);

    for (int i = 0; morse[i] != '\0'; i++) {
        switch (morse[i]) {
            case '.':
                buzzer_on();
                HAL_Delay(MORSE_UNIT);
                buzzer_off();
                HAL_Delay(MORSE_UNIT);
                break;
            case '-':
                buzzer_on();
                HAL_Delay(MORSE_UNIT * 3);
                buzzer_off();
                HAL_Delay(MORSE_UNIT);
                break;
            case ' ':
                HAL_Delay(MORSE_UNIT * 3);
                break;
            case '/':
                HAL_Delay(MORSE_UNIT * 7);
                break;
        }
    }

    //buzzer_actif = 0;
}

/* Lit un message caractère par caractère depuis la liaison série (UART2). S'arrête sur '\n', '\r', '\0' ou Ctrl+Z */
void lecture_message(char *buffer, uint16_t buffer_size) {
    uint8_t c;
    uint16_t i = 0;

    /* Attendre la fin d'une éventuelle émission en cours */
    while (buzzer_actif) {
        HAL_Delay(10);
    }

    /* Vider les caractères reçus pendant l'émission (résidus dans le buffer UART) */
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)) {
        HAL_UART_Receive(&huart2, &c, 1, 0);
    }

    /* Lire les caractères un par un jusqu'à la fin de ligne ou le buffer plein */
    while (i < buffer_size - 1) {
        HAL_UART_Receive(&huart2, &c, 1, HAL_MAX_DELAY);

        if (c == '\n' || c == '\r' || c == '\0' || c == 26) {
            /* Fin de saisie : on envoie un retour à la ligne pour l'affichage PC */
            HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
            break;
        }
        else if (c == 0x7F || c == 0x08) {
            /* Backspace : on efface le dernier caractère dans le terminal et dans le buffer */
            if (i > 0) {
                i--;
                HAL_UART_Transmit(&huart2, (uint8_t *)"\b \b", 3, HAL_MAX_DELAY);
            }
        }
        else {
            /* Caractère normal : on l'affiche en écho et on le stocke dans le buffer */
            HAL_UART_Transmit(&huart2, &c, 1, HAL_MAX_DELAY);
            buffer[i++] = c;
        }
    }
    buffer[i] = '\0';
}