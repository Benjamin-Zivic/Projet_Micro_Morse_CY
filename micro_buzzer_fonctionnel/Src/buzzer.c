#include "buzzer.h"

/* Active le buzzer en démarrant le signal PWM sur le canal 1 de TIM1 */
void buzzer_on(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

/* Coupe le buzzer en arrêtant le signal PWM */
void buzzer_off(void) {
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

/* Change la fréquence du son en modifiant la période du timer.
   Le registre ARR (AutoReload) fixe la durée d'un cycle PWM.
   On remet le rapport cyclique à 50% pour garder un son équilibré. */
void buzzer_set_freq(uint32_t freq_hz) {
    //uint32_t period = (84000000 / (42 * freq_hz)) - 1;
    uint32_t period = (1000000 / freq_hz) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim1, period);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, period / 2); /* rapport cyclique 50% */
}

/* Règle le volume en ajustant le rapport cyclique du PWM.
   Un rapport élevé = impulsion longue = son plus fort.
   On plafonne à 99% pour éviter un rapport de 100% qui bloquerait le signal. */
void buzzer_set_volume(uint8_t percent) {
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
    //uint32_t pulse = (period * percent) / 100;
    uint32_t pulse = (period * (percent < 100 ? percent : 99)) / 100;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
}