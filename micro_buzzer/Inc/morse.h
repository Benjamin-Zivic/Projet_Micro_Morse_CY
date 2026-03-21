/*
 * morse.h
 *
 *  Created on: 18 mars 2026
 *      Author: PC
 */

#ifndef INC_MORSE_H_
#define INC_MORSE_H_

#include "main.h"
#define MORSE_UNIT 200	// vitesse = 200 ms

extern UART_HandleTypeDef huart2;
extern volatile uint8_t buzzer_actif;

void morse_init(void);
void text_to_morse(const char *text, char *output, uint16_t output_size);
void buzzer_morse(const char *morse);
void lecture_message(char *buffer, uint16_t buffer_size);

#endif /* INC_MORSE_H_ */
