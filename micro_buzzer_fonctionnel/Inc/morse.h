#ifndef INC_MORSE_H_
#define INC_MORSE_H_

#include "main.h"

/* Durée de base du morse en millisecondes */
#define MORSE_UNIT 60

/* Handle UART2 utilisé pour recevoir les messages depuis le PC */
extern UART_HandleTypeDef huart2;

/* Initialise le buzzer et charge la table morse */
void morse_init(void);

/* Convertit une chaîne de texte en sa représentation morse sous forme de chaîne */
void text_to_morse(const char *text, char *output, uint16_t output_size);

/* Joue une chaîne morse sur le buzzer (points, traits et silences) */
void buzzer_morse(const char *morse);

/* Lit un message envoyé par le PC via la liaison série (UART2) et le stocke dans buffer */
void lecture_message(char *buffer, uint16_t buffer_size);

#endif