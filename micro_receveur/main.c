/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Récepteur Morse avec debug ADC
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "morse_receiver.h"
#include "config.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
MorseReceiver receiver;

volatile uint16_t g_adc_val = 0;
volatile uint8_t g_adc_new = 0;
volatile uint8_t g_adc_div = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
int _write(int file, char *ptr, int len);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  morse_receiver_init(&receiver);
  HAL_TIM_Base_Start_IT(&htim2);

  printf("\r\n==============================\r\n");
  printf("Recepteur Morse demarre\r\n");
  printf("==============================\r\n");
  /* USER CODE END 2 */

  char received_msg[MORSE_MSG_MAX_LEN] = {0};
  uint8_t received_len = 0;

  while (1)
  {
    /* USER CODE BEGIN 3 */

    if (g_adc_new)
    {
        g_adc_new = 0;

        // traitement du signal
    }

    /* Si le décodeur a ajouté une nouvelle lettre, on l'affiche et on la sauvegarde */
    if (receiver.decoder.message_len > received_len)
    {
        char new_char = receiver.decoder.message[receiver.decoder.message_len - 1];
        printf("RECU: %c\r\n", new_char);

        received_msg[received_len++] = new_char;
        received_msg[received_len] = '\0';
    }

    /* Si le message est complet, on l'affiche en entier puis on réinitialise */
    if (receiver.decoder.message_ready)
    {
        printf("RECU: %s\r\n", received_msg);

        received_len = 0;
        memset(received_msg, 0, sizeof(received_msg));

        morse_receiver_init(&receiver);
    }

    /* USER CODE END 3 */
  }
}

/* ================= CLOCK ================= */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;

  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

/* ================= ADC ================= */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;

  HAL_ADC_Init(&hadc1);

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/* ================= TIM2 ================= */
static void MX_TIM2_Init(void)
{
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = TIM2_PRESCALER;
  htim2.Init.Period =  TIM2_PERIOD;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

  HAL_TIM_Base_Init(&htim2);
}

/* ================= UART ================= */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.Mode = UART_MODE_TX_RX;
  HAL_UART_Init(&huart2);
}

/* ================= GPIO ================= */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
}

/* ================= TIMER CALLBACK ================= */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2) return;

    HAL_ADC_Start(&hadc1); /* Lance une conversion ADC */

    if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
    {
        g_adc_val = HAL_ADC_GetValue(&hadc1); /* Récupère la valeur convertie */

        /* Affichage 1 fois sur 10 */
        g_adc_div++;
        if (g_adc_div >= 10)
        {
            g_adc_div = 0;
            g_adc_new = 1;
        }

        morse_receiver_update(&receiver, g_adc_val, HAL_GetTick());
    }
}


/* ================= ERROR ================= */
void Error_Handler(void)
{
  while (1);
}
