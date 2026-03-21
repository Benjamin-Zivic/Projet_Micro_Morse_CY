/*
 * morse.c
 *
 *  Created on: 18 mars 2026
 *      Author: PC
 */

#include "morse.h"
#include "table_morse.h"
#include "buzzer.h"
#include <string.h>

static MorseTable table;
volatile uint8_t buzzer_actif = 0;

void morse_init(void) {
    morse_table_load(&table);  // charge la table depuis la Flash
}

void text_to_morse(const char *text, char *output, uint16_t output_size) {
    output[0] = '\0';

    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        if (c == ' ') {
            strncat(output, "/ ", output_size - strlen(output) - 1);
            continue;
        }

        MorseCode code;
        if (morse_lookup_char(&table, c, &code) == MORSE_OK) {

            // convertit MorseCode en string ".-"
            for (uint8_t j = 0; j < code.length; j++) {
                char sym[2] = { code.symbols[j] == MORSE_DOT ? '.' : '-', '\0' };
                strncat(output, sym, output_size - strlen(output) - 1);
            }
            strncat(output, " ", output_size - strlen(output) - 1);
        }
        // caractère non trouvé est ignoré
    }
}

void buzzer_morse(const char *morse) {
    buzzer_actif = 1;  // ← bloque le clavier

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
                HAL_Delay(MORSE_UNIT * 2);
                break;
            case '/':
                HAL_Delay(MORSE_UNIT * 6);
                break;
        }
    }

    buzzer_actif = 0;  // ← débloque le clavier
}

void lecture_message(char *buffer, uint16_t buffer_size) {
    uint8_t c;
    uint16_t i = 0;

    // vide le buffer UART au cas où des touches ont été tapées pendant le buzzer
    while (buzzer_actif) {
        HAL_Delay(10);
    }

    // vide les caractères reçus pendant la transmission
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)) {
        HAL_UART_Receive(&huart2, &c, 1, 0);
    }

    while (i < buffer_size - 1) {
        HAL_UART_Receive(&huart2, &c, 1, HAL_MAX_DELAY);

        if (c == '\n' || c == '\r' || c == '\0' || c == 26) {
            // fin de message → retour à la ligne
            HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
            break;
        }
        else if (c == 0x7F || c == 0x08) {
            // backspace ou DEL
            if (i > 0) {
                i--;  // supprime le dernier caractère du buffer

                // efface le caractère dans le terminal
                HAL_UART_Transmit(&huart2, (uint8_t *)"\b \b", 3, HAL_MAX_DELAY);
            }
        }
        else {
            // caractère normal → echo + ajout au buffer
            HAL_UART_Transmit(&huart2, &c, 1, HAL_MAX_DELAY);
            buffer[i++] = c;
        }
    }
    buffer[i] = '\0';
}
