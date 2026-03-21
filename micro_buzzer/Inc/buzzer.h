/*
 * buzzer.h
 *
 *  Created on: 18 mars 2026
 *      Author: PC
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"
extern TIM_HandleTypeDef htim2;

void buzzer_on(void);
void buzzer_off(void);
void buzzer_set_freq(uint32_t freq_hz);
void buzzer_set_volume(uint8_t percent);


#endif /* INC_BUZZER_H_ */
