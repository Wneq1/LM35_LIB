/*
 * LM35.c
 *
 *  Created on: Jul 4, 2023
 *      Author: Kuba
 */
#include "LM35.h"
#include "stm32l1xx_hal.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart2;

void ADC_Init(void)
{

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfDiscConversion = 0;


  HAL_ADC_Init(&hadc);
}

void UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;


  HAL_UART_Init(&huart2);
}

void DisplayTemperature(uint32_t temperature, uint8_t decimal_places, const char* text)
{
  char format_str[30];
  sprintf(format_str, "%s T: %%d.%%0%dd\r\n", text, decimal_places);

  uint32_t integer_part = temperature / 100;
  uint32_t fractional_part = temperature % 100;

  for (uint8_t i = 0; i < decimal_places; i++) {
    fractional_part *= 10;
  }

  fractional_part /= 100;

  char char_num_var[30];
  sprintf(char_num_var, format_str, integer_part, fractional_part);

  HAL_UART_Transmit(&huart2, (uint8_t*)char_num_var, strlen(char_num_var), HAL_MAX_DELAY);
}

uint32_t GetTemperature(uint8_t ADC_channel)
{
	char error_message[50];
  ADC_ChannelConfTypeDef sConfig = {0};


  switch (ADC_channel) {
    case 0:
      sConfig.Channel = ADC_CHANNEL_0;
      break;
    case 1:
      sConfig.Channel = ADC_CHANNEL_1;
      break;
    case 2:
      sConfig.Channel = ADC_CHANNEL_2;
      break;
    case 3:
      sConfig.Channel = ADC_CHANNEL_3;
      break;
    case 4:
      sConfig.Channel = ADC_CHANNEL_4;
      break;
    case 5:
      sConfig.Channel = ADC_CHANNEL_5;
      break;
    case 6:
      sConfig.Channel = ADC_CHANNEL_6;
      break;
    case 7:
      sConfig.Channel = ADC_CHANNEL_7;
      break;
    case 8:
      sConfig.Channel = ADC_CHANNEL_8;
      break;
    case 9:
      sConfig.Channel = ADC_CHANNEL_9;
      break;
    case 10:
      sConfig.Channel = ADC_CHANNEL_10;
      break;
    case 11:
      sConfig.Channel = ADC_CHANNEL_11;
      break;
    case 12:
      sConfig.Channel = ADC_CHANNEL_12;
      break;
    case 13:
      sConfig.Channel = ADC_CHANNEL_13;
      break;

    default:
      sprintf(error_message, "Błędny numer kanału ADC: %d\r\n", ADC_channel);
      HAL_UART_Transmit(&huart2, (uint8_t*)error_message, strlen(error_message), HAL_MAX_DELAY);
      return 0;
  }


  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES;



  HAL_ADC_Init(&hadc);
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  uint32_t ADC_val;

  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, 1000);
  ADC_val = HAL_ADC_GetValue(&hadc);
  HAL_ADC_Stop(&hadc);

  uint32_t temp = (ADC_val * 330) / 4095;
  uint32_t temp_decimal = ((ADC_val * 330) % 4095) * 100 / 4095;

  uint32_t temperature = temp * 100 + temp_decimal;
  return temperature;
}

void hysteresis(uint32_t temp, uint32_t hist, uint32_t currentTemp, GPIO_TypeDef* gpio1, uint16_t pin1, GPIO_TypeDef* gpio2, uint16_t pin2, GPIO_TypeDef* gpio3, uint16_t pin3){
	 if (currentTemp <= (temp - hist)) {
	    // Wartość temperatury jest poniżej niż dolny próg histerezy

	    HAL_GPIO_WritePin(gpio1, pin1, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(gpio2, pin2, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(gpio3, pin3, GPIO_PIN_RESET);
	  } else if (currentTemp >= (temp + hist)) {
	    // Wartość temperatury jest powyżej niż górny próg histerezy

	    HAL_GPIO_WritePin(gpio1, pin1, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(gpio2, pin2, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(gpio3, pin3, GPIO_PIN_RESET);
	  } else {
	    // Wartość temperatury mieści się w zakresie histerezy

	    HAL_GPIO_WritePin(gpio1, pin1, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(gpio2, pin2, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(gpio3, pin3, GPIO_PIN_SET);
	  }
	 char uart_buffer[50];
	  sprintf(uart_buffer, "Dolna granica histerezy: %d\r\n", temp - hist);
	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
	  sprintf(uart_buffer, "Ustawiona temp: %d\r\n", temp);
	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
	  sprintf(uart_buffer, "Gorna histereza: %d\r\n",temp + hist);
	  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
	}
