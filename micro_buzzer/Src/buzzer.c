/*
 * buzzer.c
 *
 *  Created on: 18 mars 2026
 *      Author: PC
 */
#include "buzzer.h"

void buzzer_on(void) {	//active le buzzer
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void buzzer_off(void) {	//desactive le buzzer
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}

void buzzer_set_freq(uint32_t freq_hz) {	// modifie la hauteur du son
    uint32_t period = (84000000 / (42 * freq_hz)) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim2, period);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, period / 2); // garde 50% duty cycle
}
void buzzer_set_volume(uint8_t percent) {  // modifie le volume (0% à 100%)
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim2);
    uint32_t pulse = (period * percent) / 100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse);
}

