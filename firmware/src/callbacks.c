/**
 ******************************************************************************
 * @file 	callbacks.c
 * @author 	Can GULMEZ
 * @brief 	Peripheral callbacks of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm1f)
{
	BaseType_t highPri = pdFALSE;

	if (hdfsdm1f->Instance == DFSDM1_Filter0)
	{
		/* set the flag here... */
		vTaskNotifyGiveFromISR(micTaskHandlers[0], &highPri);
	}
	else if (hdfsdm1f->Instance == DFSDM1_Filter1)
	{
		/* set the flag here... */
		vTaskNotifyGiveFromISR(micTaskHandlers[1], &highPri);
	}
	else if (hdfsdm1f->Instance == DFSDM1_Filter2)
	{
		/* set the flag here... */
		vTaskNotifyGiveFromISR(micTaskHandlers[2], &highPri);
	}
	else if (hdfsdm1f->Instance == DFSDM1_Filter3)
	{
		/* set the flag here... */
		vTaskNotifyGiveFromISR(micTaskHandlers[3], &highPri);
	}
	portYIELD_FROM_ISR(highPri);
}
