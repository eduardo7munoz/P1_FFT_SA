/*
 * ADC.c
 *
 * this file contains the initialization for the ADC
 * this ADC was put in continuous conversion mode
 * no interrupts were used here
 * it was configured for circular mode in the DMA
 * the adc here is triggered off the input from timer2 it triggers conversions on both rising and falling
 *  Created on: Apr 16, 2023
 *      Author: eduardomunoz
 */
#include "ADC.h"

void ADC_init(void)
{
    //enable ADC on RCC
    RCC->AHB2ENR |= (RCC_AHB2ENR_ADCEN);
    //set ADC to use HCLK
    ADC123_COMMON-> CCR = (ADC123_COMMON->CCR & ~(ADC_CCR_CKMODE))|
            (1<<ADC_CCR_CKMODE_Pos);

    //take ADC out of deep power down mode
    ADC1->CR &= ~(ADC_CR_DEEPPWD);
    //and turn on the voltage regulator
    ADC1->CR |= (ADC_CR_ADVREGEN);

    //    delay_us(20);  //wait 20us for ADC to power up
    HAL_Delay(1);


    //single ended calibration, ensure ADC is disabled
    ADC1->CR &= ~(ADC_CR_ADEN |ADC_CR_ADCALDIF);
    ADC1->CR |= (ADC_CR_ADCAL);
    while(ADC1->CR & ADC_CR_ADCAL); //wait for ADCAL to become

    //configure single ended for channel 5
    ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);

    //enable ADC FINALLY!!!!! (software procedure to enable the ADC)
    ADC1-> ISR |= (ADC_ISR_ADRDY);
    ADC1->CR   |= (ADC_CR_ADEN);
    while(!(ADC1->ISR & ADC_ISR_ADRDY)); //wait for ADRDY to become
    ADC1-> ISR |= ADC_ISR_ADRDY; //clear ADRDY bit

    //configure ADC
    //12-bit resolution so set to 00
    ADC1 -> CFGR &= ~(ADC_CFGR_RES);
    ADC1 -> CFGR |= (ADC_CFGR_DMACFG); //enable dma circ mode
    ADC1 -> CFGR |= (ADC_CFGR_DMAEN); //enable dma


    //trigger off of extsel
    ADC1 -> CFGR |= ADC_CFGR_EXTEN_0|ADC_CFGR_EXTEN_1;//enables triggers of external rising edge
    ADC1 -> CFGR &= ~(ADC_CFGR_EXTSEL); //clear bits
    ADC1 -> CFGR |= (ADC_CFGR_EXTSEL_0|ADC_CFGR_EXTSEL_1); //EXT3 TIM2CH2 011

    //sampling time on channel 5 is 2.5 clocks
    ADC1->SMPR1 &= ~(ADC_SMPR1_SMP5);
    ADC1->SMPR1 |= (ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP5_1 |ADC_SMPR1_SMP5_2);

    //put channel 5 in the regular sequence, length of 1
    ADC1->SQR1 = (ADC1->SQR1 & ~(ADC_SQR1_SQ1 | ADC_SQR1_L))|
            (5 << ADC_SQR1_SQ1_Pos);

    //configure GPIO PA0 for analog input
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
    GPIOA ->MODER |= (GPIO_MODER_MODE0); //analog mode
    GPIOA ->ASCR |= GPIO_ASCR_ASC0;      //connect analog PA0

//    //start regular sequence
//    ADC1->CR |= ADC_CR_ADSTART;
}
