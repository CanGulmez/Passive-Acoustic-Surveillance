/**
 ******************************************************************************
 * @file 	tasks.c
 * @author 	Can Gulmez
 * @brief 	FreeRTOS tasks of the firmware.
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

/**
 * Read microphone data from the north channel.
 */
void taskMicSensorNorth(void *pvParams)
{
	int32_t i, notified;
	static int32_t samples[SAMPLE_SIZE] = {0};
	HAL_StatusTypeDef status;
	
	printLog("I'm taskMicSensorNorth() task!");

	/* Start the DFSDM1 filter 1 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f[0], samples, SAMPLE_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 1 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < SAMPLE_SIZE; i++)
			{
				samples[i] = samples[i] >> 24;	/* 8-bit MSB */
			}
			/* Take the mutex to update global payload object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				for (i = 0; i < DATA_SIZE; i++)
				{
					payloadData.micNorth[i] = samples[i * 2];
					payloadData.micNorthEast[i] = samples[i * 2 + 1];
				}
				/* Give up the mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read microphone data from the east channel.
 */
void taskMicSensorEast(void *pvParams)
{
	int32_t i, notified;
	static int32_t samples[SAMPLE_SIZE] = {0};
	HAL_StatusTypeDef status;

	printLog("I'm taskMicSensorEast() task!");

	/* Start the DFSDM1 filter 2 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f[1], samples, SAMPLE_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 2 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */	
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < SAMPLE_SIZE; i++)
			{
				samples[i] = samples[i] >> 24;	/* 8-bit MSB */
			}
			/* Take the mutex to update global payload object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				for (i = 0; i < DATA_SIZE; i++)
				{
					payloadData.micEast[i] = samples[i * 2];
					payloadData.micSouthEast[i] = samples[i * 2 + 1];
				}
				/* Give up the mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read microphone data from the south channel.
 */
void taskMicSensorSouth(void *pvParams)
{
	int32_t i, notified;
	static int32_t samples[SAMPLE_SIZE] = {0};
	HAL_StatusTypeDef status;

	printLog("I'm taskMicSensorSouth() task!");

	/* Start the DFSDM1 filter 3 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f[2], samples, SAMPLE_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 3 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < SAMPLE_SIZE; i++)
			{
				samples[i] = samples[i] >> 24;	/* 8-bit MSB */
			}
			/* Take the mutex to update global payload object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				for (i = 0; i < DATA_SIZE; i++)
				{
					payloadData.micSouth[i] = samples[i * 2];
					payloadData.micSouthWest[i] = samples[i * 2 + 1];
				}
				/* Give up the mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read microphone data from the west channel.
 */
void taskMicSensorWest(void *pvParams)
{
	int32_t i, notified;
	static int32_t samples[SAMPLE_SIZE] = {0};
	HAL_StatusTypeDef status;

	printLog("I'm taskMicSensorWest() task!");

	/* Start the DFSDM1 filter 4 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f[3], samples, SAMPLE_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 4 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < SAMPLE_SIZE; i++)
			{
				samples[i] = samples[i] >> 24;	/* 8-bit MSB */
			}
			/* Take the mutex to update global payload object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				for (i = 0; i < DATA_SIZE; i++)
				{
					payloadData.micWest[i] = samples[i * 2];
					payloadData.micNorthWest[i] = samples[i * 2 + 1];
				}
				/* Give up the mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read IMU data from the existing sensor.
 */
void taskIMUSensor(void *pvParams)
{
	uint8_t whoami;

	printLog("I'm taskIMUSensor() task!");

	/* Confirm that IMU sensor is registered. */
	whoami = __read_reg_from_imu(IMU_REG_WHO_AM_I);
	if (whoami != 0x6A)
		printLog("Failed to confirm the IMU sensor!");

	printLog("Confirmed the existing of IMU sensor.");

	/* Configure the accelerometer and gyroscope. */
	__write_to_imu_reg(IMU_REG_CTRL1_XL, 0x6C);	/* 416Hz, 16g */
	__write_to_imu_reg(IMU_REG_CTRL2_G, 0x6C);	/* 416Hz, 2000dps */
	__write_to_imu_reg(IMU_REG_CTRL3_C, 0x44);	/* BDU and IF_INC */

	printLog("Configured the accelerometer (416Hz, 16g) "
				"and gyroscope (416Hz, 2000dps).");

	for (;;)
	{
		/* Take the mutex to update global payload object. */
		if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
		{
			/* Read the acceleromenter, gyroscope and temperature. */
			__read_accel_from_imu(&payloadData);
			__read_gyro_from_imu(&payloadData);
			__read_temp_from_imu(&payloadData);

			/* Give up the mutex. */
			xSemaphoreGive(payloadMutex);
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read GPS data from the existing module.
 */
void taskGPSModule(void *pvParams)
{
	uint8_t buffer[DATA_SIZE] = {0};
	HAL_StatusTypeDef status;

	printLog("I'm taskGPSModule() task!");

	for (;;)
	{
		/* Recieve the GPS sentences over serial line. */
		status = HAL_UART_Receive(&huart7, buffer, DATA_SIZE, HAL_MAX_DELAY);
		if (status != HAL_OK)
			printError(status, "Failed to receive GPS sentences!\n");
		
		/* Take the mutex to update shared variable. */
		if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
		{
			/* Parse the NMEA sentences. */
			__parse_nmea_sentences(buffer, &payloadData);

			/* Give up the mutex. */
			xSemaphoreGive(payloadMutex);
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Write the backup data into existing SD card.
 */
void taskSDCard(void *pvParams)
{
	uint8_t buffer[DATA_SIZE] = {0};
	HAL_StatusTypeDef status;
	static uint32_t csector = START_SECTOR;

	printLog("I'm taskSDCard() task!");

	/* Initialize the SD card handler. */
	status = HAL_SD_Init(&hsdmmc1);
	if (status != HAL_OK)
		printError(status, "Failed to initialize SD card!");

	/* Get the SD card information. */
	__get_sd_card_info();

	for (;;)
	{
		/* Fill the backup buffer (will be implemented later). */
		
		/* Write the backup data blocks to SD card. */
		status = HAL_SD_WriteBlocks(
			&hsdmmc1,			/* SD card interface */
			buffer,				/* data that will be written */
			csector,				/* current sector number */
			1,						/* just 1 sector at each op. */
			HAL_MAX_DELAY		/* give some time */
		);
		if (status != HAL_OK)
			printError(status, "Failed to write data blocks to "
				"SD Card at sector %lu!", csector);

		csector++;
	}
	vTaskDelete(NULL);
}

/**
 * Transmit the LoRa package down to ground.
 */
void taskLoRaModule(void *pvParams)
{
	PayloadData temp;

	printLog("I'm taskLoRaModule() task!");

	for (;;)
	{
		/* Take the mutex to update global payload object. */
		if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
		{
			memcpy(&temp, &payloadData, sizeof(payloadData));	/* for debugging */
			
			/* Give up the mutex. */
			xSemaphoreGive(payloadMutex);
		}
		/* Transmit the packed structure to ground station. */
		HAL_UART_Transmit(&huart5, (uint8_t *) &temp, sizeof(temp), HAL_MAX_DELAY);
	}	
	vTaskDelete(NULL);
}

/**
 * Check the system healthy with predefined conditions.
 */
void taskSystemCheck(void *pvParams)
{
	printLog("I'm taskSystemCheck() task!");

	for (;;)
	{
		
	}	
	vTaskDelete(NULL);
}

/**
 * Blink the LEDs to give visual insights.
 */
void taskLEDs(void *pvParams)
{
	printLog("I'm taskLEDs() task!");

	for (;;)
	{		
		
	}	
	vTaskDelete(NULL);
}

/**
 * Handle the watch dog to maintain system wakeup. 
 */
void taskWatchdog(void *pvParams)
{
	printLog("I'm taskWatchDog() task!");
	
	for (;;)
	{		
		
	}	
	vTaskDelete(NULL);	
}

/**
 * Application Idle Hook
 */
void vApplicationIdleHook(void)
{
	__WFI();		/* enter into deep sleep! */
}
