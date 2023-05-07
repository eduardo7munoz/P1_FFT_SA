/*
 * GPIO.c
 *
 * this file contains the initialization for GPIOC pins utilized for timing measurements
 *
 *  Created on: Apr 15, 2023
 *      Author: eduardomunoz
 */
#include "GPIO.h"
#include "main.h"

void GPIO_init()
{

	//setup on PC0 for toggling output
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN); //enable clock
	GPIOC->MODER &= ~(GPIO_MODER_MODE0); //clearing bits setting to input
	GPIOC->MODER |= (GPIO_MODER_MODE0_0); //setting output;
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR0); //clear bits
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0); //clear bit, set to push pull

	GPIOC->MODER &= ~(GPIO_MODER_MODE1); //clearing bits setting to input
	GPIOC->MODER |= (GPIO_MODER_MODE1_0); //setting output;
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR1); //clear bits
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT1); //clear bit, set to push pull


}
