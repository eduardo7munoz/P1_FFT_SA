/*
 * DMA.h
 *
 *  Created on: Apr 15, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_DMA_H_
#define SRC_DMA_H_

//in order to maintain functionality with 4096 SAMPLINGFREQ the FFTSIZE must not fall below 512
//in order to maintain functinality with 2048 SAMPLINGFREQ the FFTSIZE must not fall below 256
//going below would require changing bins to 64, and offsetting printing of magnitude by 2 spaces per print
#define LENGTH_OF_ARRAY 4096
#define LENGTH_OF_FFT 512
#define HANNING_SIZE LENGTH_OF_FFT
#define NUM_TO_COPY LENGTH_OF_ARRAY

#include "main.h"
#include "arm_math.h"



void DMA1channel3_mem2mem_init(uint32_t *, uint32_t *);
void DMA1channel1_ADC_init(uint32_t *);

void hanning(q15_t *window, uint16_t length);
void applyhanning(q15_t *samples, uint16_t length);


#endif /* SRC_DMA_H_ */
