#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"

/* TIM1 est utilisé pour générer le signal PWM qui pilote le buzzer */
extern TIM_HandleTypeDef htim1;

/* Active/coupe le buzzer */
void buzzer_on(void);
void buzzer_off(void);

/* Change la fréquence du son émis (en Hz) */
void buzzer_set_freq(uint32_t freq_hz);

/* Règle le volume du buzzer de 0 % à 100 % */
void buzzer_set_volume(uint8_t percent);

#endif