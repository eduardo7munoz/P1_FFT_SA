/*
 * MyTasks.c
 * This file holds the tasks utilized to calculate and print the spectrum analysis
 * The tasks utilize semaphores to synchronize calculations, printing, and to protect the information in buffers
 *
 *
 *
 *  Created on: Apr 12, 2023
 *      Author: eduardomunoz
 */

#include <stdlib.h>
#include "cmsis_os.h"
#include "main.h"
#include "MyTasks.h"
#include "DMA.h"
#include "GPIO.h"
#include "ADC.h"
#include "UART.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "semphr.h"
#include "UART_plotter.h"

#define SIZEOUTPUT_FFTSA LENGTH_OF_FFT*2

char freqstring[4];
extern uint16_t collectedsamples[LENGTH_OF_ARRAY]; //declared in main
extern arm_rfft_instance_q15 fft1; //declared in main

void setupFFT(void *argument)
{
	vTaskSuspend(calculate_freq_Handler);
	vTaskSuspend(print_freq_Handler);

	GPIO_init();
	ADC_init();
	DMA1channel1_ADC_init(collectedsamples);
	tim2_init();
	UART_Init();
	UART_escapes("[H");
	UART_print("Frequency: ");
	UART_escapes("[s");


	arm_rfft_init_q15(&fft1, LENGTH_OF_FFT, 0, 1);

	vTaskResume(calculate_freq_Handler);
	vTaskResume(print_freq_Handler);


	vTaskDelete(setup_FFT_Handler);

}

SemaphoreHandle_t printSemaphore = NULL;
SemaphoreHandle_t calculateSemaphore1 = NULL;
SemaphoreHandle_t calculateSemaphore2 = NULL;
q15_t testOutput[SIZEOUTPUT_FFTSA/2] = {0};
q15_t fftNormalized[SIZEOUTPUT_FFTSA/2] = {0};
q15_t max;

/*
 * This task calculates the FFT for Half and Full transfers of the DMA
 * These calculations are synchronized using calculateSemaphore1 and calculateSemaphore2
 * Since these are binary semaphores attempting to take a semaphore will block the task for the alotted time
 * when the the semaphore is given in the ISRs the task is able to resume
 * this task also gives the print semaphore once a calculation is done
 */

void calculate(void *argument)
{


	arm_rfft_instance_q15 fft1;
	uint16_t freq;

	uint32_t maxindex;
	q15_t * sampleloc1 =  (q15_t *)&collectedsamples;
	q15_t * sampleloc2 =  (q15_t *)&collectedsamples[2047];
	arm_rfft_init_q15(&fft1, LENGTH_OF_FFT, 0, 1);

	printSemaphore = xSemaphoreCreateBinary();//creating the semaphore once

	calculateSemaphore1 = xSemaphoreCreateBinary();
	calculateSemaphore2 = xSemaphoreCreateBinary();

	if(calculateSemaphore1 != 	NULL && calculateSemaphore2!= NULL && printSemaphore!= NULL)
	{
		for(;;)
		{

			q15_t * outputloc = &testOutput[0];
			q15_t * maxfreq = &testOutput[1];
		    //start regular sequence
		    ADC1->CR |= ADC_CR_ADSTART;

				if(xSemaphoreTake(calculateSemaphore1, ( TickType_t ) 400 ) == pdTRUE)
				{



//					arm_fir_f32();
					applyhanning(sampleloc1, HANNING_SIZE); //used to reduce spectral leakage

					arm_rfft_q15(&fft1, sampleloc1, outputloc); //conducts fft on result of hanning window

					arm_max_q15(&testOutput[1], SIZEOUTPUT_FFTSA/4-1 ,&max ,&maxindex); //sampling rate was 4.096Khz so 1kHz in bottom 1/4



					freq = (maxindex+1);
					GPIOC->ODR ^= GPIO_ODR_OD1;

					itoa(freq*4, freqstring, 10);
					xSemaphoreGive(printSemaphore);



				}

				if(xSemaphoreTake(calculateSemaphore2, ( TickType_t ) 400 )== pdTRUE)
				{

					applyhanning(sampleloc2, HANNING_SIZE);

					arm_rfft_q15(&fft1, sampleloc2, outputloc);//conducts fft on result of hanning window


					arm_max_q15(&testOutput[1], SIZEOUTPUT_FFTSA/4-1 ,&max ,&maxindex); //sampling rate was 4.096Khz so 1kHz in bottom 1/4

					freq = (maxindex+1);
					itoa(freq*4, freqstring, 10);
					xSemaphoreGive(printSemaphore);


				}


		}
	}
}

/*
 * this task is used to print out the spectral data after calculations
 * by using a binary semaphore the task is blocked when it attempts to take the semaphore and it is not available
 * upon being given in the calculate task after an fft calculation it will return here and unblock the task
 */

void print(void *argument)
{


	printgraph();
	printnumbers();


	for(;;)
	{

		if(xSemaphoreTake(printSemaphore, ( TickType_t ) 1200) == pdTRUE)
		{
			GPIOC->ODR ^=GPIO_ODR_OD0;
			eraseplot();
			printmag(max);
			UART_escapes("[H");
			UART_escapes("[11C");
			UART_escapes("[K");
			UART_print(freqstring);


		}

	}

}

