/*
 * ADC.h
 *
 *  Created on: Apr 16, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_
#include "main.h"

void SysTick_Init(void);

void delay_us(const uint16_t time_us);

void ADC_init();

#endif /* SRC_ADC_H_ */
