/*
 * UART.h
 *
 *  Created on: Apr 3, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_UART_H_
#define SRC_UART_H_

#include "timer.h" //this has frequency of internal clock

#define FREQ FREQUENCY
#define BAUD_RATE 9600
#define BRR_VALUE FREQ/BAUD_RATE


void UART_Init();
void UART_print(char *outputstring);
void USART2_IRQHandler();
void UART_escapes(char *escstring);


#endif /* SRC_UART_H_ */
