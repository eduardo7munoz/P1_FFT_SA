/*
 * MyTasks.h
 *
 *  Created on: Apr 12, 2023
 *      Author: eduardomunoz
 */
#include "cmsis_os.h"
#include "main.h"
#ifndef SRC_MYTASKS_H_
#define SRC_MYTASKS_H_

TaskHandle_t setup_FFT_Handler,calculate_freq_Handler, print_freq_Handler;

void setupFFT(void *argument);

void calculate(void *argument);

void print(void *argument);

#endif /* SRC_MYTASKS_H_ */
