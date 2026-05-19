/**
 ******************************************************************************
 * @file 	it.c
 * @author 	Can Gulmez
 * @brief 	Interrupt service routines of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can Gulmez.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 *****************************************************************************
 */

#include "main.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	
	if ( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) 
	{
		xPortSysTickHandler();
	}
}

void DMA1_Stream0_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdfsdm1dma[0]);
}

void DMA1_Stream1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdfsdm1dma[1]);
}

void DMA1_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdfsdm1dma[2]);
}

void DMA1_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdfsdm1dma[3]);
}
