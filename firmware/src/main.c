/**
 ******************************************************************************
 * @file 	main.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Main source file of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Ahmet Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

int main(void)
{
	BaseType_t status;

	HAL_Init();
	
	/* Enable the peripheral clocks. */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_UART4_CLK_ENABLE();
	__HAL_RCC_UART5_CLK_ENABLE();
	__HAL_RCC_UART7_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_SDMMC1_CLK_ENABLE();
	__HAL_RCC_DFSDM1_CLK_ENABLE();

	/* Initialize and configure the peripherals. */
	configOscClk();
	configDebugPort();
	configIMUSensor();
	configMicSensors();
	configGPSModule();
	configSDCard();
	configLoRaModule();
	configLEDs();
	configWatchdog();

	/* Create the mutex for the payload data. */
	payloadMutex = xSemaphoreCreateMutex();
	if (payloadMutex == NULL)
	{
		printKernel("Failed to create the payload mutex!");
	}

	/* Create the taskMicSensorNorth() task. */
	status = xTaskCreate(taskMicSensorNorth, TASK_MIC_NAME_N, 
		TASK_MIC_STACK, NULL, TASK_MIC_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_N);
	}

	/* Create the taskMicSensorEast() task. */
	status = xTaskCreate(taskMicSensorEast, TASK_MIC_NAME_E, 
		TASK_MIC_STACK, NULL, TASK_MIC_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_E);
	}

	/* Create the taskMicSensorSouth() task. */
	status = xTaskCreate(taskMicSensorSouth, TASK_MIC_NAME_S, 
		TASK_MIC_STACK, NULL, TASK_MIC_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_S);
	}

	/* Create the taskMicSensorWest() task. */
	status = xTaskCreate(taskMicSensorWest, TASK_MIC_NAME_W, 
		TASK_MIC_STACK, NULL, TASK_MIC_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_W);
	}

	/* Create the taskGPSModule() task. */
	status = xTaskCreate(taskGPSModule, TASK_GPS_NAME, 
		TASK_GPS_STACK, NULL, TASK_GPS_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_GPS_NAME);
	}

	/* Create the taskIMUSensor() task. */
	status = xTaskCreate(taskIMUSensor, TASK_IMU_NAME, 
		TASK_IMU_STACK, NULL, TASK_IMU_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_IMU_NAME);
	}

	/* Create the taskSDCard() task. */
	status = xTaskCreate(taskSDCard, TASK_SD_NAME, 
		TASK_SD_STACK, NULL, TASK_SD_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_SD_NAME);
	}

	/* Create the taskServoMotors() task. */
	status = xTaskCreate(taskServoMotors, TASK_SERVO_NAME, 
		TASK_SERVO_STACK, NULL, TASK_SERVO_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_SERVO_NAME);
	}

	/* Create the taskLoRaModule() task. */
	status = xTaskCreate(taskLoRaModule, TASK_LORA_NAME, 
		TASK_LORA_STACK, NULL, TASK_LORA_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_LORA_NAME);
	}

	/* Create the taskSystemCheck() task. */
	status = xTaskCreate(taskSystemCheck, TASK_SYSTEM_NAME, 
		TASK_SYSTEM_STACK, NULL, TASK_SYSTEM_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_SYSTEM_NAME);
	}

	/* Create the taskLEDs() task. */
	status = xTaskCreate(taskLEDs, TASK_LED_NAME, 
		TASK_LED_STACK, NULL, TASK_LED_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_LED_NAME);
	}

	/* Create the taskWatchDog() task. */
	status = xTaskCreate(taskWatchdog, TASK_WATCHDOG_NAME, 
		TASK_WATCHDOG_STACK, NULL, TASK_WATCHDOG_PRIORITY, NULL);
	if (status != pdPASS)
	{
		printKernel("Failed to create '%s' task!", TASK_WATCHDOG_NAME);
	}

	/* Start the task schedular. */
	vTaskStartScheduler();

	while (1);
}
 