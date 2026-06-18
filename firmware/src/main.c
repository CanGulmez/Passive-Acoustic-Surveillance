/**
 ******************************************************************************
 * @file 	main.c
 * @author 	Can Gulmez
 * @brief 	Main source file of the firmware.
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

TaskHandle_t micTaskHandlers[CHANNEL_COUNT] = {0};

int main(void)
{
	HAL_StatusTypeDef status;

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
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* Initialize and configure the peripherals. */
	configOscClk();
	configSerialLine();
	configMicSensors();
	configIMUSensor();
	// configSDWriting();

	// printLog("\nThe firmware is running...");
	// printLog("System core clock is %ld Hz", SystemCoreClock);

	/* Create the mutex to protect the payload data. */
	payloadMutex = xSemaphoreCreateMutex();
	if (payloadMutex == NULL)
		printKernel("Failed to create the payload mutex!");

	/* Create the payload event group. */
	payloadEvent = xEventGroupCreate();
	if (payloadEvent == NULL)
		printKernel("Failed to create the payload event group!");

	/* Create the taskMicSensor0() task. */
	status = xTaskCreate(taskMicSensor0, TASK_MIC_NAME_0, TASK_MIC_STACK, 
		NULL, TASK_MIC_PRIORITY, &micTaskHandlers[0]);
	if (status != pdPASS)
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_0);

	/* Create the taskMicSensor1() task. */
	status = xTaskCreate(taskMicSensor1, TASK_MIC_NAME_1, TASK_MIC_STACK, 
		NULL, TASK_MIC_PRIORITY, &micTaskHandlers[1]);
	if (status != pdPASS)
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_1);

	/* Create the taskMicSensor2() task. */
	status = xTaskCreate(taskMicSensor2, TASK_MIC_NAME_2, TASK_MIC_STACK, 
		NULL, TASK_MIC_PRIORITY, &micTaskHandlers[2]);
	if (status != pdPASS)
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_2);

	/* Create the taskMicSensor2() task. */
	status = xTaskCreate(taskMicSensor3, TASK_MIC_NAME_3, TASK_MIC_STACK, 
		NULL, TASK_MIC_PRIORITY, &micTaskHandlers[3]);
	if (status != pdPASS)
		printKernel("Failed to create '%s' task!", TASK_MIC_NAME_3);

	/* Create the taskIMUSensor() task. */
	status = xTaskCreate(taskIMUSensor, TASK_IMU_NAME, TASK_IMU_STACK, 
		NULL, TASK_IMU_PRIORITY, NULL);
	if (status != pdPASS)
		printKernel("Failed to create '%s' task!", TASK_IMU_NAME);

	/* Create the taskTransmitter() task. */
	status = xTaskCreate(taskTransmitter, TASK_SERIAL_NAME, TASK_SERIAL_STACK, 
		NULL, TASK_SERIAL_PRIORITY, NULL);
	if (status != pdPASS)
		printKernel("Failed to create '%s' task!", TASK_SERIAL_NAME);

	/* Create the taskSDCardWriting() task. */
	// status = xTaskCreate(taskSDWriting, TASK_SD_NAME, TASK_SD_STACK, 
	// 	NULL, TASK_SD_PRIORITY, NULL);
	// if (status != pdPASS)
	// 	printKernel("Failed to create '%s' task!", TASK_SD_NAME);

	// printLog("free heap size is %d bytes", xPortGetFreeHeapSize());

	/* Start the task schedular. */
	vTaskStartScheduler();
}
 