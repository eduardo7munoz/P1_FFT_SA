/*
 * UART_plotter.h
 *
 *  Created on: May 7, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_UART_PLOTTER_H_
#define SRC_UART_PLOTTER_H_
#include "main.h"
#include <stdlib.h>
#define VERTICAL_HEIGHT 30

void printmag(uint16_t max);

void eraseplot();

void printgraph();


#endif /* SRC_UART_PLOTTER_H_ */
