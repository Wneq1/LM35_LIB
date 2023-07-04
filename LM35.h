/*
 * LM35.h
 *
 *  Created on: Jul 4, 2023
 *      Author: Kuba
 */

#ifndef LM35_H_
#define LM35_H_


#include <stdint.h>
#include "stm32l1xx_hal.h"

void DisplayTemperature(uint32_t temperature, uint8_t decimal_places, const char* text);
void hysteresis(uint32_t temp, uint32_t hist, uint32_t currentTemp, GPIO_TypeDef* gpio1, uint16_t pin1, GPIO_TypeDef* gpio2, uint16_t pin2, GPIO_TypeDef* gpio3, uint16_t pin3);
uint32_t GetTemperature(uint8_t ADC_channel);
#endif /* LM35_H_ */
