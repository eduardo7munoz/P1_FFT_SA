/*
 * timer.c
 *
 *  Created on: Apr 22, 2023
 *      Author: eduardomunoz
 */

#include "timer.h"
#include "main.h"

/*
 * setup timer 2
 */
void tim2_init()
{
	//capture compare mode ch2
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // enable clock for tim2

	TIM2->CCER |= (TIM_CCER_CC2E);        // enable capture compare
	TIM2->CCER &= ~(TIM_CCER_CC2P);       // rising edge non inverted
	TIM2->CCER &= ~(TIM_CCER_CC2NP);
	TIM2->CCR2 = 0;

	TIM2->CCMR1 &= ~(TIM_CCMR1_CC2S); // set as output

	TIM2-> CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0); //

	TIM2->ARR = ARR_VALUE; //setting

	TIM2->CR1 |= (TIM_CR1_CEN);

}


