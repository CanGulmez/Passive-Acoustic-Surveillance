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
		vTaskNotifyGiveFromISR(micTaskHandlers[0], &highPri);
	}
	else if (hdfsdm1f->Instance == DFSDM1_Filter1)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[1], &highPri);
	}
	else if (hdfsdm1f->Instance == DFSDM1_Filter2)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[2], &highPri);
	}
	else if (hdfsdm1f->Instance == DFSDM1_Filter3)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[3], &highPri);
	}
	portYIELD_FROM_ISR(highPri);
}
