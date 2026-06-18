/**
 ******************************************************************************
 * @file 	main.h
 * @author 	Can Gulmez
 * @brief 	Main headers of the firmware.
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

#ifndef ACOUSTIC_SURVEILLANCE_FIRMWARE_H
#define ACOUSTIC_SURVEILLANCE_FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Libraries */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

#include "peripheral.h"
#include "kernel.h"

#include "ff.h"

/* Global and generic definitions */

#define BUFFER_SIZE			512
#define DATA_SIZE			1024	/* 1 KB */
#define CHANNEL_COUNT		4
#define MIC_COUNT			(CHANNEL_COUNT * 2)
#define MAGIC_WORD			0xDEADBEEF

#define FILE				__FILE__
#define LINE				__LINE__

#define STATUS(status) 		((status == HAL_ERROR) ? "ERROR" : 		\
							 (status == HAL_BUSY) ? "BUSY" : 		\
							 (status == HAL_TIMEOUT) ? "TIMEOUT" : 	\
							  "UNDEFINED")

#define MIC_IRQ				configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1
#define SD_IRQ				configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2

#define PACKED				__attribute__((packed, aligned(1)))
#define BACKUP				__attribute__((section(".axi_sram"), aligned(32)))

/*****************************************************************************/

/* Peripheral handlers and kernel associated objects */

extern RCC_OscInitTypeDef iosc;				/* Oscillator */
extern RCC_ClkInitTypeDef iclk;				/* Clock */
extern GPIO_InitTypeDef	igpio;				/* Generic IO */

extern UART_HandleTypeDef huart4;			/* Debug Port */

extern RCC_PeriphCLKInitTypeDef hdfsdm1clk;
extern DFSDM_Channel_HandleTypeDef hdfsdm1c2;
extern DFSDM_Channel_HandleTypeDef hdfsdm1c3;
extern DFSDM_Channel_HandleTypeDef hdfsdm1c4;
extern DFSDM_Channel_HandleTypeDef hdfsdm1c7;
extern DFSDM_Filter_HandleTypeDef hdfsdm1f0;
extern DFSDM_Filter_HandleTypeDef hdfsdm1f1;
extern DFSDM_Filter_HandleTypeDef hdfsdm1f2;
extern DFSDM_Filter_HandleTypeDef hdfsdm1f3;
extern DMA_HandleTypeDef hdfsdm1dma0;
extern DMA_HandleTypeDef hdfsdm1dma1;
extern DMA_HandleTypeDef hdfsdm1dma2;
extern DMA_HandleTypeDef hdfsdm1dma3;

extern SPI_HandleTypeDef hspi1;				/* IMU Sensor */

extern SD_HandleTypeDef	hsdmmc1;			/* SD Card */
extern MPU_Region_InitTypeDef hsdmmc1mpu;	/* SD Card MPU */
extern RCC_PeriphCLKInitTypeDef hsdmmc1clk;	/* SD Card Clock */

extern TaskHandle_t micTaskHandlers[CHANNEL_COUNT];
extern SemaphoreHandle_t payloadMutex;
extern EventGroupHandle_t payloadEvent;

typedef struct PACKED _PayloadData
{
	/* microphone sensors payload data */

	int32_t micFilter0[DATA_SIZE];
	int32_t micFilter1[DATA_SIZE];
	int32_t micFilter2[DATA_SIZE];
	int32_t micFilter3[DATA_SIZE];

	/* IMU sensor payload data */

	float imuAccel[3];				/* [x, y, z] m/s^2 */
	float imuGyro[3];				/* [x, y, z] dps */
	float imuTemp;					/* C */

} PayloadData;

extern PayloadData payloadData;

/*****************************************************************************/

/* Printing macro functions over serial line */

#define printLog(format, ...)												\
do {																		\
	char buffer[BUFFER_SIZE];												\
																			\
	snprintf(buffer, BUFFER_SIZE, format "\r\n", ##__VA_ARGS__);			\
	HAL_UART_Transmit(&huart4, (uint8_t *) buffer, strlen(buffer), 			\
		HAL_MAX_DELAY);														\
} while (0)

#define printError(status, format, ...)										\
do {																		\
	char buffer[BUFFER_SIZE];												\
																			\
	snprintf(buffer, BUFFER_SIZE, "*** " format " (STATUS = %s) " 			\
		"(%s::%d) ***\r\n", ##__VA_ARGS__, STATUS(status), FILE, LINE);		\
	HAL_UART_Transmit(&huart4, (uint8_t *) buffer, strlen(buffer), 			\
		HAL_MAX_DELAY);														\
} while(0)

#define printKernel(format, ...)											\
do {																		\
	char buffer[BUFFER_SIZE];												\
																			\
	snprintf(buffer, BUFFER_SIZE, "*** " format " (%s::%d) ***\r\n", 		\
		##__VA_ARGS__, FILE, LINE);											\
	HAL_UART_Transmit(&huart4, (uint8_t *) buffer, strlen(buffer), 			\
		HAL_MAX_DELAY);														\
} while (0)

/*****************************************************************************/

/* Dummy syscall prototypes */

extern int _read(int, char *, int);
extern int _write(int, char *, int);
extern int _lseek(int, int, int);
extern int _close(int);
extern int _getpid(void);
extern int _kill(int);
extern int _fstat(int, struct stat *);
extern int _isatty(int);

/* Configuration function prototypes */

extern void configOscClk(void);
extern void configSerialLine(void);
extern void configMicSensors(void);
extern void configIMUSensor(void);
extern void configSDWriting(void);

/* RTOS task function prototypes */

extern void taskMicSensor0(void *);
extern void taskMicSensor1(void *);
extern void taskMicSensor2(void *);
extern void taskMicSensor3(void *);
extern void taskIMUSensor(void *);
extern void taskTransmitter(void *);
extern void taskSDWriting(void *);

/* HAL interrupt request function prototypes */

extern void SysTick_Handler(void);
extern void DMA1_Stream0_IRQHandler(void);
extern void DMA1_Stream1_IRQHandler(void);
extern void DMA1_Stream2_IRQHandler(void);
extern void DMA1_Stream3_IRQHandler(void);
extern void SDMMC1_IRQHandler(void);

/* HAL callback function prototypes */

extern void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *);
extern void HAL_SD_TxCpltCallback(SD_HandleTypeDef *);
extern void HAL_SD_RxCpltCallback(SD_HandleTypeDef *);

/* RTOS interrupt handler function prototypes */

extern void xPortSysTickHandler(void);
extern void vApplicationIdleHook(void);

/* Helper and utility function prototypes */

extern void writeRegToIMU(uint8_t, uint8_t);
extern uint8_t readRegFromIMU(uint8_t);
extern void readAccelFromIMU(PayloadData *);
extern void readGyroFromIMU(PayloadData *);
extern void readTempFromIMU(PayloadData *);

/*****************************************************************************/

/* Inline functions */

/**
 * Initialize the generic GPIO handler.
 */
static inline void initGPIO(GPIO_InitTypeDef *handle, uint32_t pin, uint32_t mode, 
	uint32_t pull, int32_t alternate)
{
	handle->Pin = pin;
	handle->Mode = mode;
	handle->Pull = pull;
	handle->Speed = GPIO_SPEED_FREQ_HIGH;
	if (alternate != -1)
	{
		handle->Alternate = alternate;
	}
}

#ifdef __cplusplus
}
#endif

#endif		/* ACOUSTIC_SURVEILLANCE_FIRMWARE */