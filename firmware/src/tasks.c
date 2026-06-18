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

/* Global and shared objects */

PayloadData payloadData = {0};
SemaphoreHandle_t payloadMutex = {0};
EventGroupHandle_t payloadEvent = {0};

/**
 * Read microphone data from the filter 0 (physically MK2).
 */
void taskMicSensor0(void *pvParams)
{
	(void)pvParams;
	
	int32_t i, notified;
	static int32_t samples[DATA_SIZE] = {0};
	HAL_StatusTypeDef status;

	/* Start the DFSDM1 filter 0 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f0, samples, DATA_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 0 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < DATA_SIZE; i++)
			{
				samples[i] = samples[i] >> 8;
			}
			/* Take the payload mutex to update the shared payload data object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				memcpy(payloadData.micFilter0, samples, DATA_SIZE * sizeof(int32_t));
				
				/* Set the payload event group bit of this task. */
				xEventGroupSetBits(payloadEvent, TASK_MIC_EVENT_BIT_0);

				/* Give up the payload mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read microphone data from the filter 1 (physically MK7).
 */
void taskMicSensor1(void *pvParams)
{
	(void)pvParams;

	int32_t i, notified;
	static int32_t samples[DATA_SIZE] = {0};
	HAL_StatusTypeDef status;

	/* Start the DFSDM1 filter 1 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f1, samples, DATA_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 1 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */	
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < DATA_SIZE; i++)
			{
				samples[i] = samples[i] >> 8;
			}
			/* Take the payload mutex to update the shared payload data object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				memcpy(payloadData.micFilter1, samples, DATA_SIZE * sizeof(int32_t));
				
				/* Set the payload event group bit of this task. */
				xEventGroupSetBits(payloadEvent, TASK_MIC_EVENT_BIT_1);

				/* Give up the payload mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read microphone data from the filter 2 (physically MK6).
 */
void taskMicSensor2(void *pvParams)
{
	(void)pvParams;

	int32_t i, notified;
	static int32_t samples[DATA_SIZE] = {0};
	HAL_StatusTypeDef status;

	/* Start the DFSDM1 filter 2 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f2, samples, DATA_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 2 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < DATA_SIZE; i++)
			{
				samples[i] = samples[i] >> 8;
			}
			/* Take the payload mutex to update the shared payload data object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				memcpy(payloadData.micFilter2, samples, DATA_SIZE * sizeof(int32_t));
				
				/* Set the payload event group bit of this task. */
				xEventGroupSetBits(payloadEvent, TASK_MIC_EVENT_BIT_2);

				/* Give up the payload mutex. */
				xSemaphoreGive(payloadMutex);
			}
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Read microphone data from the filter 3 (physically MK5).
 */
void taskMicSensor3(void *pvParams)
{
	(void)pvParams;

	int32_t i, notified;
	static int32_t samples[DATA_SIZE] = {0};
	HAL_StatusTypeDef status;

	/* Start the DFSDM1 filter 3 in DMA mode. */
	status = HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1f3, samples, DATA_SIZE);
	if (status != HAL_OK)
		printError(status, "Failed to start DFSDM filter 3 DMA conversion!");

	for (;;)
	{
		/* Wait the full conversion is complete by the its callback. */
		notified = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (notified != 0)
		{
			for (i = 0; i < DATA_SIZE; i++)
			{
				samples[i] = samples[i] >> 8;
			}
			/* Take the payload mutex to update the shared payload data object. */
			if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
			{
				memcpy(payloadData.micFilter3, samples, DATA_SIZE * sizeof(int32_t));
				
				/* Set the payload event group bit of this task. */
				xEventGroupSetBits(payloadEvent, TASK_MIC_EVENT_BIT_3);

				/* Give up the payload mutex. */
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
	(void)pvParams;

	uint8_t whoami;
	TickType_t lastWake;

	/* Confirm that IMU sensor is registered. */
	whoami = readRegFromIMU(IMU_REG_WHO_AM_I);
	if (whoami != 0x6A)
		printLog("Failed to confirm the IMU sensor!");

	/* Configure the accelerometer and gyroscope. */
	writeRegToIMU(IMU_REG_CTRL1_XL, 0x66);	/* 416Hz, 16g */
	writeRegToIMU(IMU_REG_CTRL2_G, 0x6C);	/* 416Hz, 2000dps */
	writeRegToIMU(IMU_REG_CTRL3_C, 0x44);	/* BDU and IF_INC */

	/* Get the recent task tick count. */
	lastWake = xTaskGetTickCount();
	for (;;)
	{
		/* Give some period. */
		vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(1000));

		/* Take the payload mutex to update the payload data. */
		if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
		{
			/* Read the acceleromenter, gyroscope and temperature. */
			readAccelFromIMU(&payloadData);
			readGyroFromIMU(&payloadData);
			readTempFromIMU(&payloadData);

			/* Set the payload event group bit of this task. */
			xEventGroupSetBits(payloadEvent, TASK_IMU_EVENT_BIT);

			/* Give up the payload mutex. */
			xSemaphoreGive(payloadMutex);
		}
	}	
	vTaskDelete(NULL);
}

/**
 * Transmit the payload data to ground station over serial line.
 */
void taskTransmitter(void *pvParams)
{
	(void)pvParams;

	int8_t magicWord = MAGIC_WORD;
	EventBits_t eventBits = (TASK_MIC_EVENT_BIT_0 | TASK_MIC_EVENT_BIT_1 |
							 TASK_MIC_EVENT_BIT_2 | TASK_MIC_EVENT_BIT_3 |
							 TASK_IMU_EVENT_BIT);
	for (;;)
	{
		/* Block to wait for the event bits to become set within the payload
		   event group. */
		xEventGroupWaitBits(
			payloadEvent,					/* event group object */
			eventBits,						/* event bits to wait for */
			pdTRUE,							/* clear the bits on exit */
			pdTRUE,							/* wait for all bits to be set */
			portMAX_DELAY					/* wait indefinitely */
		);
		/* Transmit payload data locally using mutex. */
		if (xSemaphoreTake(payloadMutex, portMAX_DELAY))
		{
			HAL_UART_Transmit(&huart4, (uint8_t *)&magicWord, sizeof(magicWord), 
				portMAX_DELAY);
			HAL_UART_Transmit(&huart4, (uint8_t *)&payloadData, sizeof(payloadData), 
				portMAX_DELAY);

			// printLog("filter0 (MK2): %ld, %ld, %ld", payloadData.micFilter0[10], payloadData.micFilter0[100], payloadData.micFilter0[500]);
			// printLog("filter1 (MK7): %ld, %ld, %ld", payloadData.micFilter1[10], payloadData.micFilter1[100], payloadData.micFilter1[500]);
			// printLog("filter2 (MK6): %ld, %ld, %ld", payloadData.micFilter2[10], payloadData.micFilter2[100], payloadData.micFilter2[500]);
			// printLog("filter3 (MK5): %ld, %ld, %ld", payloadData.micFilter3[10], payloadData.micFilter3[100], payloadData.micFilter3[500]);
			// printLog("accel: %f, %f, %f", payloadData.imuAccel[0], payloadData.imuAccel[1], payloadData.imuAccel[2]);
			// printLog("gyro: %f, %f, %f", payloadData.imuGyro[0], payloadData.imuGyro[1], payloadData.imuGyro[2]);
			// printLog("temp: %f", payloadData.imuTemp);
			
			/* Give up the payload mutex. */
			xSemaphoreGive(payloadMutex);
		}
	}
	vTaskDelete(NULL);
}

/**
 * Back up the sensor data into the on-board SD card.
 */
void taskSDWriting(void *pvParams)
{
	(void)pvParams;

	/**
	 * Some important notes about SDMMC1 and SD Card backing up:
	 * 
	 * + SDMMC1 can only access AXI (D1) SRAM (0x24000000), 
	 *   not SRAM1/2/3. Place all DMA buffers there.
	 * + All DMA buffers must be 32-byte aligned.
	 * + SDMMC1 uses DMA internally. Disable cache for the DMA 
	 *   region over MPU.
	 */
	
	static int32_t filter0[DATA_SIZE] BACKUP;
	static int32_t filter1[DATA_SIZE] BACKUP;
	static int32_t filter2[DATA_SIZE] BACKUP;
	static int32_t filter3[DATA_SIZE] BACKUP;
	static float imu[7] BACKUP;		/* 3 accel, 3 gyro, 1 temp */
	
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
 
