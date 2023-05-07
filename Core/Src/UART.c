/*
 * UART.c
 * This file is used to initialize the UART properly
 * This also handles functionality for receiving input from vt100 terminal and transmitting
 *
 *  Created on: Apr 3, 2023
 *      Author: eduardomunoz
 */
#include "main.h"
#include "UART.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>



void UART_Init()
{

	//Will be utilizing GPIOA Ports A2(TX) and A3(RX) for USART2
	GPIOA->AFR[0] =  (GPIOA->AFR[0]&~(GPIO_AFRL_AFSEL3_Msk|GPIO_AFRL_AFSEL2_Msk)) | (0x77<<GPIO_AFRL_AFSEL2_Pos);; //makes it AF7 for usart2
	GPIOA->MODER = ((GPIOA->MODER) &~ (GPIO_MODER_MODE3_Msk|GPIO_MODER_MODE2_Msk))|(0b1010<<GPIO_MODER_MODE2_Pos); //set as AF
	GPIOA->PUPDR = GPIOA->PUPDR &~ (GPIO_PUPDR_PUPD3_Msk|GPIO_PUPDR_PUPD2_Msk); // set pull-up pull-down to 00
	GPIOA->OTYPER = GPIOA->OTYPER &~ (GPIO_OTYPER_OT3_Msk|GPIO_OTYPER_OT2_Msk); // set push-pull

	RCC->APB1ENR1 |= (RCC_APB1ENR1_USART2EN_Msk);

	//Declare the length of the transmitted word: 8 for ascii
	USART2->CR1 = USART2->CR1 &~ (USART_CR1_M1_Msk|USART_CR1_M0_Msk); //set 0 0 for length 8
	//Declare desired baud rate: for 4M/9600 = 416 also oversampling by 16
	USART2->BRR = BRR_VALUE;
	//Declare the amount of stop bits: 1
	USART2->CR2 &= ~(USART_CR2_STOP_Msk);

	//Usart Enable Bit
	USART2->CR1 |= (USART_CR1_UE);

	//Transmit And Receive Enable Bits
	USART2->CR1 |= (USART_CR1_TE|USART_CR1_RE);

	//Receive interrupt enable
	USART2->CR1 |= (USART_CR1_RXNEIE);

	//enable USART2 ISR in NVIC
	NVIC->ISER[1] = (1 << (USART2_IRQn & 0x1F));


	__enable_irq();
}

void UART_print(char *outputstring)
{
	for(uint16_t currchar = 0; currchar<strlen(outputstring);currchar++)
	{
		while(!(USART2->ISR & USART_ISR_TXE)); //Transmit data register empty, using negation since value of 0 means data has not been transferred

		USART2->TDR = outputstring[currchar];
	}

}

void USART2_IRQHandler()
{
	uint8_t RX = USART2->RDR;
	char tempstr[6]={'\0'};
	switch(RX)
	{
		case '\r':
			UART_escapes("[1B");//move down 1 line
			UART_escapes("\r"); //carriage return

			break;
		case 'J':
			UART_escapes("[2J"); //clear everything
			break;

		default:

			if(RX-0x30>=0 && RX-0x30<=9)
			{

			}
				break;
	}
}

void UART_escapes(char *escstring)
{
	while(!(USART2->ISR & USART_ISR_TXE));//do not advance until receiving transmit flag
	USART2->TDR = (0x1B);  // ESC
	UART_print(escstring);

}


