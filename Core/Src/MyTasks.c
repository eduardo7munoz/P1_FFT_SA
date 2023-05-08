/*
 * MyTasks.c
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

#define SIZEFFT_FOR_SA LENGTH_OF_FFT
#define SIZEOUTPUT_FFTSA SIZEFFT_FOR_SA*2

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


	arm_rfft_init_q15(&fft1, SIZEFFT_FOR_SA, 0, 1);

	vTaskResume(calculate_freq_Handler);
	vTaskResume(print_freq_Handler);


	vTaskDelete(setup_FFT_Handler);

}

SemaphoreHandle_t printSemaphore = NULL;
SemaphoreHandle_t calculateSemaphore1 = NULL;
SemaphoreHandle_t calculateSemaphore2 = NULL;
q15_t testOutput[SIZEOUTPUT_FFTSA/2] = {0};
q15_t max;

void calculate(void *argument)
{

//	xQueue1 = xQueueCreate( 10, sizeof(uint16_t) );

//	if( xQueue1 == NULL)
//	{
//	     while(1);/* Queue was not created and must not be used. */
//	}

	arm_rfft_instance_q15 fft1;
	uint16_t freq;

	uint32_t maxindex;
	q15_t * sampleloc1 =  (q15_t *)&collectedsamples;
	q15_t * sampleloc2 =  (q15_t *)&collectedsamples[2047];
	arm_rfft_init_q15(&fft1, SIZEFFT_FOR_SA, 0, 1);

	printSemaphore = xSemaphoreCreateBinary();//creating the semaphore once

	calculateSemaphore1 = xSemaphoreCreateBinary();
	calculateSemaphore2 = xSemaphoreCreateBinary();


	if(printSemaphore != NULL && calculateSemaphore1 !=NULL)
	{
		for(;;)
		{

			q15_t * outputloc = &testOutput[0];
			q15_t * maxfreq = &testOutput[1];
		    //start regular sequence
		    ADC1->CR |= ADC_CR_ADSTART;

				if(xSemaphoreTake(calculateSemaphore1, ( TickType_t ) 400 ) == pdTRUE)
				{


//					applyhanning(sampleloc1, HANNING_SIZE);

					arm_rfft_q15(&fft1, sampleloc1, outputloc);



					arm_max_q15(&testOutput[1], SIZEOUTPUT_FFTSA/2-1 ,&max ,&maxindex);



					freq = (maxindex+1);
					GPIOC->ODR ^= GPIO_ODR_OD1;

					itoa(freq*8/2, freqstring, 10);
					xSemaphoreGive(printSemaphore);



				}

				if(xSemaphoreTake(calculateSemaphore2, ( TickType_t ) 400 )== pdTRUE)
				{

//					applyhanning(sampleloc2, HANNING_SIZE);
					arm_rfft_q15(&fft1, sampleloc2, outputloc);


					arm_max_q15(&testOutput[1], SIZEOUTPUT_FFTSA/2-1 ,&max ,&maxindex);


					freq = (maxindex+1);
					itoa(freq*8/2, freqstring, 10);
					xSemaphoreGive(printSemaphore);


				}



//		xQueueSend(xQueue1, (void *) freq, ( TickType_t ) 10);
//		vTaskDelay(10/portTICK_PERIOD_MS);


		}
	}
}

void print(void *argument)
{
	printgraph();
	printnumbers();
	eraseplot();

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

