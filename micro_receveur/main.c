/* Dans main.c — boucle principale */

MorseReceiver receiver;
morse_receiver_init(&receiver);

while (1) {
    uint16_t adc_val = /* résultat HAL_ADC_GetValue(&hadc1) */;
    uint32_t now     = HAL_GetTick();

    morse_receiver_update(&receiver, adc_val, now);

    if (morse_receiver_is_done(&receiver)) {
        const char *msg = morse_receiver_get_message(&receiver);
        /* Envoyer msg via UART : HAL_UART_Transmit(...) */
    }
}