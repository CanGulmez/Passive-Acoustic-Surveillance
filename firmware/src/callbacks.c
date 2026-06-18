/**
 ******************************************************************************
 * @file 	callbacks.c
 * @author 	Can Gulmez
 * @brief 	Peripheral callbacks of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can Gulmez.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdmf)
{
	BaseType_t highPri = pdFALSE;

	if (hdfsdmf->Instance == DFSDM1_Filter0)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[0], &highPri);
	}
	else if (hdfsdmf->Instance == DFSDM1_Filter1)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[1], &highPri);
	}
	else if (hdfsdmf->Instance == DFSDM1_Filter2)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[2], &highPri);
	}
	else if (hdfsdmf->Instance == DFSDM1_Filter3)
	{
		vTaskNotifyGiveFromISR(micTaskHandlers[3], &highPri);
	}
	portYIELD_FROM_ISR(highPri);
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsdmmc)
{
    (void)hsdmmc;
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsdmmc)
{
    (void)hsdmmc;
}
