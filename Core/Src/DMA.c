/*
 * DMA.c
 * this file provides 2 initialization functions
 * 1 for memory to memory transfers
 * and another for peripheral to memory transfers utilizing the ADC
 * the number to copy #define can be found in the DMA.h file and modified there
 * this file also contains the IRQ handlers for both of the channels that were utilized
 *  Created on: Apr 15, 2023
 *      Author: eduardomunoz
 */
#include "main.h"
#include "DMA.h"
#include "cmsis_os.h"
#include "semphr.h"

extern SemaphoreHandle_t calculateSemaphore1;
extern SemaphoreHandle_t calculateSemaphore2;

void DMA1channel3_mem2mem_init(uint32_t *source, uint32_t *dest)
{
	//enabling clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	//selecting DMA1 channel 3

    //memory to memory initialization
	DMA1_Channel3->CPAR = (uint32_t)(source); //with DIR set to 0 this is the source address
	DMA1_Channel3->CMAR = (uint32_t)dest; // with DIR set to 0 this is the destination address
	DMA1_Channel3->CNDTR = NUM_TO_COPY;
	DMA1_Channel3->CCR |= (DMA_CCR_MINC);
	DMA1_Channel3->CCR |= (DMA_CCR_PINC);
	DMA1_Channel3->CCR = ((DMA1_Channel3->CCR)&~(DMA_CCR_MSIZE))
							|(DMA_CCR_MSIZE_0);//clearing size and setting length to 16bits
	DMA1_Channel3->CCR = ((DMA1_Channel3->CCR)&~(DMA_CCR_PSIZE))
							|(DMA_CCR_PSIZE_0);
	DMA1_Channel3->CCR &= ~(DMA_CCR_DIR); //setting DIR to 0
	DMA1_Channel3->CCR |= (DMA_CCR_MEM2MEM); //enabling memory to memory
	DMA1_Channel3->CCR |= (DMA_CCR_TCIE); //enable interrupts at transfer complete

	NVIC->ISER[0] = (1<<(DMA1_Channel3_IRQn & 0x1F)); //enable interrupts for channel 3
	__enable_irq();




}

/*
 * for  DMA1channel1_ADC_init(uint32_t * dest)
 * interrupts are enabled in order to allow for synchronizing calculations with semaphores
 */

void DMA1channel1_ADC_init(uint32_t * dest)
{
	//enabling clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    //memory to memory initialization
	DMA1_CSELR->CSELR &= ~(DMA_CSELR_C1S); //select dma channel
	DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR); //with DIR set to 0 this is the source address
	DMA1_Channel1->CMAR = (uint32_t)(dest); // with DIR set to 0 this is the destination address
	DMA1_Channel1->CNDTR = NUM_TO_COPY;  // number to copy
	DMA1_Channel1->CCR &= ~(DMA_CCR_DIR); //setting DIR to 0
	DMA1_Channel1->CCR |= (DMA_CCR_CIRC); //enable circular mode
	DMA1_Channel1->CCR |= (DMA_CCR_MINC); //increments address
	DMA1_Channel1->CCR &= ~(DMA_CCR_PINC);//keeps address the same
	DMA1_Channel1->CCR = ((DMA1_Channel1->CCR)&~(DMA_CCR_MSIZE))|(DMA_CCR_MSIZE_0);//clearing size and setting length to 16bits
	DMA1_Channel1->CCR = ((DMA1_Channel1->CCR)&~(DMA_CCR_PSIZE))|(DMA_CCR_PSIZE_0);//clearing size and setting length to 16bits

	DMA1_Channel1->CCR |= (DMA_CCR_TCIE); //enable interrupts at transfer complete
	DMA1_Channel1->CCR |= (DMA_CCR_HTIE); //enable interrupts at half transfer


	NVIC->ISER[0] = (1<<(DMA1_Channel1_IRQn & 0x1F)); //enable interrupts for channel 1

	//this mcu uses 4 bits for priority level, uses msb
//	NVIC->IP[DMA1_Channel1_IRQn] = (0x70 << ((DMA1_Channel1_IRQn%4)*8)); //modulus since the remainder determines position in register for priority
	NVIC_SetPriority(DMA1_Channel1_IRQn, 0x19);
	__enable_irq();

	DMA1_Channel1->CCR |= (DMA_CCR_EN); //activating channel

}



void DMA1_Channel3_IRQHandler()
{
	if(DMA1->ISR & DMA_ISR_TCIF3)
	{
	  	GPIOC->ODR ^= GPIO_ODR_OD1; //toggle off
		  DMA1_Channel3->CCR &= ~(DMA_CCR_EN); //disable channel 3
		  DMA1->IFCR |= (DMA_IFCR_CTCIF3); //clear interrupt flag
		  DMA1->IFCR |= (DMA_IFCR_CGIF3); //clear global interrupt flag
		  DMA1->IFCR |= (DMA_IFCR_CHTIF3); //clear half transfer flag

	}
}


void DMA1_Channel1_IRQHandler()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(DMA1->ISR & DMA_ISR_HTIF1)
	{
		DMA1->IFCR |= (DMA_IFCR_CHTIF1); //clear half transfer flag


		xSemaphoreGiveFromISR(calculateSemaphore1, &xHigherPriorityTaskWoken );
	}
	else if(DMA1->ISR & DMA_ISR_TCIF1)
	{

		  DMA1->IFCR |= (DMA_IFCR_CTCIF1); //clear interrupt flag
		  DMA1->IFCR |= (DMA_IFCR_CGIF1); //clear global interrupt flag
		  xSemaphoreGiveFromISR(calculateSemaphore2, &xHigherPriorityTaskWoken );

	}

}
