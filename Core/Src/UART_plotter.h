/*
 * UART_plotter.h
 *
 *  Created on: May 7, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_UART_PLOTTER_H_
#define SRC_UART_PLOTTER_H_
#include "main.h"
#include "DMA.h"
#include <stdlib.h>
#define VERTICAL_HEIGHT 30

//in order to maintain functionality with 4096 SAMPLINGFREQ the FFTSIZE must not fall below 512
//in order to maintain functinality with 2048 SAMPLINGFREQ the FFTSIZE must not fall below 256
//going below would require changing bins to 64, and offsetting printing of magnitude by 2 spaces per print
#define BINS 128
#define BIN_MULTIPLER (FFTSIZE/2)/BINS
#define DESIREDFREQ 1024
#define BLOCK_DIVIDER ((SAMPLINGFREQ/DESIREDFREQ)*BINS) //currently 4*128
#define BLOCK_SIZE LENGTH_OF_FFT/BLOCK_DIVIDER  //currently 512/512

void printmag(uint16_t max);

void eraseplot();

void printgraph();


#endif /* SRC_UART_PLOTTER_H_ */
