/*
 * DMA.h
 *
 *  Created on: Apr 15, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_DMA_H_
#define SRC_DMA_H_
#define LENGTH_OF_ARRAY 4096
#define LENGTH_OF_FFT 256
#define HANNING_SIZE LENGTH_OF_FFT
#define NUM_TO_COPY LENGTH_OF_ARRAY
#define ARM_MATH_CM4

#include "main.h"
#include "arm_math.h"

void DMA1channel3_mem2mem_init(uint32_t *, uint32_t *);
void DMA1channel1_ADC_init(uint32_t *);

void hanning(q15_t *window, uint16_t length);
void applyhanning(q15_t *samples, uint16_t length);


#endif /* SRC_DMA_H_ */
