/*
 * timer.h
 *
 *  Created on: Apr 22, 2023
 *      Author: eduardomunoz
 */

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

//in order to maintain functionality with 4096 SAMPLINGFREQ the FFTSIZE must not fall below 512
//in order to maintain functinality with 2048 SAMPLINGFREQ the FFTSIZE must not fall below 256
//going below would require changing bins to 64, and either offsetting printing of magnitude by 2 spaces per print

#define FREQUENCY 80000000
#define SAMPLINGFREQ 4096
#define ARR_VALUE FREQUENCY/SAMPLINGFREQ

void tim2_init();


#endif /* SRC_TIMER_H_ */
