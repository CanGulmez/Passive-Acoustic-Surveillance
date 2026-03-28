/**
 ******************************************************************************
 * @file 	it.c
 * @author 	Can GULMEZ
 * @brief 	Interrupt service routines of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can GULMEZ.
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
