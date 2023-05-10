/*
 * UART_plotter.c
 *
 *	This file contains the code for constructing the border of the Spectrum Analyzer as well as
 *	printing the magnitudes of each bin from the FFT
 *  Created on: May 7, 2023
 *      Author: eduardomunoz
 */

#include "DMA.h"
#include "UART_plotter.h"
#include "UART.h"

extern q15_t testOutput[1024];

void printmag(uint16_t lmax)
{
	q15_t outputmag[2] = {0};
	uint8_t mag = 0;

	UART_escapes("[H");
	UART_escapes("[4C");
	UART_escapes("[3B");
	UART_escapes("[2C");
	UART_escapes("[29B");
	UART_escapes("[s");


	if(testOutput[0]>0)
	{
		for(mag = 0; mag<(testOutput[0]*10)/(testOutput[0]); mag++)
		{
			UART_print("*");
			UART_escapes("[1A");
			UART_escapes("[1D");

		}
	}



	for(uint16_t index = 0; index< BINS; index++)
	{
		arm_mean_q15( &testOutput[(index * 2*BLOCK_SIZE)+2], BLOCK_SIZE, &outputmag[0]);
		outputmag[0] = ((30*(int16_t)outputmag[0])/lmax);
		outputmag[0] = outputmag[0] > 25 ? 22:outputmag[0];
		UART_escapes("8");
		UART_escapes("[C");
		UART_escapes("[s");



		for(mag = 0; mag<outputmag[0]; mag++)
		{
			UART_print("*");
			UART_escapes("[1A");
			UART_escapes("[1D");
		}

	}

}


void eraseplot()
{
	UART_escapes("[H");
	UART_escapes("[4C");
	UART_escapes("[3B");
	UART_escapes("[2C");
	//erases all previous magnitudes and ignores the borders
	for(uint16_t verticalpos = 0; verticalpos <VERTICAL_HEIGHT; verticalpos++)
		{
			UART_escapes("[K");
			UART_escapes("[1B");


		}
}



void printgraph()
{
	UART_escapes("[H");

	//[nB down n lines, [nA up n lines, [nC right n lines, [nD left n lines
	UART_escapes("[4C");
	UART_escapes("[3B");
	UART_escapes("[1C");
	UART_escapes("[s");
	UART_escapes("[1D");
	//printing the vertical axis
	for(uint16_t verticalborder = 0; verticalborder <VERTICAL_HEIGHT; verticalborder++)
	{
		UART_print("|");
		UART_escapes("[1B");
		UART_escapes("[1D");


	}
	UART_escapes("[2C");
	//printing the horizontal axis
	for(uint16_t horizontalborder = 0; horizontalborder <128; horizontalborder++)
	{


			UART_print("_");

	}

}

void printnumbers()
{
	uint16_t freq = 128;
	char freqstr1[4];


	//[nB down n lines, [nA up n lines, [nC right n lines, [nD left n lines
	UART_escapes("[126D");
	UART_escapes("[1B");


	//places freq 128-1024 inc by 128 13 spaces apart
	for(uint8_t placenum=0; placenum<8; placenum++)
	{
		itoa(freq,freqstr1, 10);
		UART_escapes("[13C");
		UART_print(freqstr1);
		freq +=128;
	}
	UART_escapes("[1B");
	UART_escapes("[80D");
	UART_print("Frequency (Hz)");

}
