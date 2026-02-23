/**
 ******************************************************************************
 * @file 	main.h
 * @author 	Ahmet Can GULMEZ
 * @brief 	Main headers of the firmware.
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

#ifndef FIRMWARE_H
#define FIRMWARE_H

/* Libraries */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>
#include <list.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>

#include <stm32h7xx_hal.h>

#include "./kernel.h"
#include "./peripheral.h"

/* Global and General Definitions */

#define BUFFER_SIZE			256
#define DATA_SIZE				512
#define GPS_SIZE				64
#define SAMPLE_SIZE			(DATA_SIZE * 2)	
#define CHANNEL_COUNT		4
#define MIC_COUNT				(CHANNEL_COUNT * 2)
#define START_SECTOR			0x1000	/* 4096 bytes, after 2MB of FAT/reserved */

#define FILE					__FILE__
#define LINE					__LINE__
#define PACKED					__attribute__((packed, aligned(1)))
#define NORETURN				__attribute__((noreturn))
#define DEPRECATED			__attribute__((deprecated))
#define ALIGNED(n)			__attribute__((aligned(n)))
#define SECTION(s)			__attribute__((section(s)))

#define STATUS(status) 		((status == HAL_ERROR) 		? "ERROR" 	: 	\
									 (status == HAL_BUSY) 		? "BUSY"		:	\
									 (status == HAL_TIMEOUT)	? "TIMEOUT" :	\
									  "UNDEFINED")

/* Some Useful Shorthands */

#define IMU_NSS_LOW()		(HAL_GPIO_WritePin(IMU_PORTA, IMU_PIN_NSS, RESET))
#define IMU_NSS_HIGH()		(HAL_GPIO_WritePin(IMU_PORTA, IMU_PIN_NSS, SET))

/*****************************************************************************/
/*****************************************************************************/

/* Peripheral Handlers */

extern RCC_OscInitTypeDef iosc;										/* Oscillator */
extern RCC_ClkInitTypeDef iclk;										/* Clock */
extern GPIO_InitTypeDef	igpio;										/* Generic IO */
extern UART_HandleTypeDef huart4;									/* Debug Port */
extern UART_HandleTypeDef huart5;									/* LoRa Module */
extern UART_HandleTypeDef huart7;									/* GPS Module */
extern SPI_HandleTypeDef hspi1;										/* IMU Sensor */
extern SD_HandleTypeDef	hsdmmc1;										/* SD Card */
extern DFSDM_Channel_HandleTypeDef hdfsdm1c[CHANNEL_COUNT];	/* Mic Sensor */
extern DFSDM_Filter_HandleTypeDef hdfsdm1f[CHANNEL_COUNT];	/* Mic Sensor */

extern SemaphoreHandle_t payloadMutex;	/* Mutex for payload data */

/*****************************************************************************/
/*****************************************************************************/

/* Data Structures and Enumerations */
typedef struct PACKED _PayloadData
{
	/* The microphone sensors payload data  */

	int8_t micNorth[DATA_SIZE];
	int8_t micNorthEast[DATA_SIZE];
	int8_t micEast[DATA_SIZE];
	int8_t micSouthEast[DATA_SIZE];
	int8_t micSouth[DATA_SIZE];
	int8_t micSouthWest[DATA_SIZE];
	int8_t micWest[DATA_SIZE];
	int8_t micNorthWest[DATA_SIZE];

	/* The GPS module payload data */

	uint8_t gpsUTCTime[GPS_SIZE];
	uint8_t gpsLatitude[GPS_SIZE];
	uint8_t gpsLongitude[GPS_SIZE];
	uint8_t gpsQuality[GPS_SIZE];
	uint8_t gpsNumSat[GPS_SIZE];
	uint8_t gpsAltitude[GPS_SIZE];
	uint8_t gpsStatus[GPS_SIZE];
	uint8_t gpsSpeed[GPS_SIZE];		/* knots */
	uint8_t gpsCourse[GPS_SIZE];		/* degrees */
	uint8_t gpsDate[GPS_SIZE];

	/* The IMU sensor payload data */
	
	float imuAccelX;						/* m/s^2 */
	float imuAccelY;						/* m/s^2 */
	float imuAccelZ;						/* m/s^2 */
	float imuGyroX;						/* dps */
	float imuGyroY;						/* dps */
	float imuGyroZ;						/* dps */
	float imuTemp;							/* C */ 
} PayloadData;

extern PayloadData payloadData;

/*****************************************************************************/
/*****************************************************************************/

/**
 * Transmit the logs from MCU to PC over serial UART line.
 */
#define printLog(format, ...)																	\
{																										\
	char buffer[BUFFER_SIZE];																	\
																										\
	snprintf(buffer, BUFFER_SIZE, format "\r\n", ##__VA_ARGS__);					\
	HAL_UART_Transmit(&huart4, buffer, strlen(buffer), HAL_MAX_DELAY);			\
}

/**
 * Transmit the HAL error from MCU to PC over serial UART line.
 */
#define printError(status, format, ...)													\
{																										\
	char buffer[BUFFER_SIZE];																	\
																										\
	snprintf(buffer, BUFFER_SIZE, "*** " format 	" (STATUS = %s) " 				\
		"(%s::%d) ***\r\n", ##__VA_ARGS__, STATUS(status), FILE, LINE);			\
	HAL_UART_Transmit(&huart4, buffer, strlen(buffer), HAL_MAX_DELAY);			\
}

/**
 * Transmit the kernel error from MCU to PC over serial UART line.
 */
#define printKernel(format, ...)																\
{																										\
	char buffer[BUFFER_SIZE];																	\
																										\
	snprintf(buffer, BUFFER_SIZE, "*** " format 	" (%s::%d) ***\r\n", 			\
		##__VA_ARGS__, FILE, LINE);															\
	HAL_UART_Transmit(&huart4, buffer, strlen(buffer), HAL_MAX_DELAY);			\
}

/*****************************************************************************/
/*****************************************************************************/

/**
 * Initialize the system oscillator with given parameters.
 */
#define initOscillator(handle, type, hsestate, pllstate, pllsrc, pllm, plln,	\
							  pllp, pllq, pllr, pllrge, pllvcosel, pllfracn)			\
{																										\
	handle.OscillatorType = type;																\
	handle.HSEState = hsestate;																\
	handle.PLL.PLLState = pllstate;															\
	handle.PLL.PLLSource = pllsrc;															\
	handle.PLL.PLLM = pllm;																		\
	handle.PLL.PLLN = plln;																		\
	handle.PLL.PLLP = pllp;																		\
	handle.PLL.PLLQ = pllq;																		\
	handle.PLL.PLLR = pllr;																		\
	handle.PLL.PLLRGE = pllrge;																\
	handle.PLL.PLLVCOSEL = pllvcosel;														\
	handle.PLL.PLLFRACN = pllfracn;															\
}

/**
 * Initialize the system clock with given parameters.
 */
#define initClock(handle, type, source, sysdiv, ahbdiv, apb1div, apb2div,		\
						apb3div, apb4div)															\
{																										\
	handle.ClockType = type;																	\
	handle.SYSCLKSource = source;																\
	handle.SYSCLKDivider = sysdiv;															\
	handle.AHBCLKDivider = ahbdiv;															\
	handle.APB1CLKDivider = apb1div;															\
	handle.APB2CLKDivider = apb2div;															\
	handle.APB3CLKDivider = apb3div;															\
	handle.APB4CLKDivider = apb4div;															\
}

/**
 * Initialize GPIO peripheral with given parameters.
 */
#define initGPIO(handle, pin, mode, pull, alternate)									\
{																										\
	handle.Pin = pin;																				\
	handle.Mode = mode;																			\
	handle.Pull = pull;																			\
	handle.Speed = GPIO_SPEED_FREQ_HIGH;													\
	if (alternate != NULL)																		\
	{																									\
		handle.Alternate = alternate;															\
	}																									\
}

/**
 * Initialize UART peripheral with given parameters.
 */
#define initUART(handle, instance, baudrate, mode, wordlen,	stopbits, 			\
					  parity, hwcontrol, sampling)											\
{																										\
	handle.Instance = instance;																\
	handle.Init.BaudRate = baudrate;															\
	handle.Init.Mode = mode;																	\
	handle.Init.WordLength = wordlen;														\
	handle.Init.StopBits = stopbits;															\
	handle.Init.Parity = parity;																\
	handle.Init.HwFlowCtl = hwcontrol;														\
	handle.Init.OverSampling = sampling;													\
}

/**
 * Initialize base TIM peripheral with given parameters.
 */
#define initBaseTIM(handle, instance, prescaler, period, clockdiv, 				\
						  countermode, repitation, autoreload)								\
{																										\
	handle.Instance = instance;																\
	handle.Init.Prescaler = prescaler;														\
	handle.Init.CounterMode = countermode;													\
	handle.Init.Period = period;																\
	handle.Init.ClockDivision = clockdiv;													\
	handle.Init.RepetitionCounter = repitation;											\
	handle.Init.AutoReloadPreload = autoreload;											\
}

/**
 * Initialize DMA peripheral with given parameters.
 */
#define initDMA(handle, instance, stream, direction, periphinc, meminc, 		\
					 periphdataalign, memdataalign, mode, priority)						\
{																										\
	handle.Instance = instance;																\
	handle.Init.Request = stream;																\
	handle.Init.Direction = direction;														\
	handle.Init.PeriphInc = periphinc;														\
	handle.Init.MemInc = meminc;																\
	handle.Init.PeriphDataAlignment = periphdataalign;									\
	handle.Init.MemDataAlignment = memdataalign;											\
	handle.Init.Mode = mode;																	\
	handle.Init.Priority = priority;															\
}

/**
 * Initialize SPI peripheral with given parameters.
 */
#define initSPI(handle, instance, mode, direction, datasize, polarity, 			\
					 phase, nss, prescaler, firstbit, timode)								\
{																										\
	handle.Instance = instance;																\
	handle.Init.Mode = mode;																	\
	handle.Init.Direction = direction;														\
	handle.Init.DataSize = datasize;															\
	handle.Init.CLKPolarity = polarity;														\
	handle.Init.CLKPhase = phase;																\
	handle.Init.NSS = nss;																		\
	handle.Init.BaudRatePrescaler = prescaler;											\
	handle.Init.FirstBit = firstbit;															\
	handle.Init.TIMode = timode;																\
}

/**
 * Initialize SDMMC peripheral with given parameters.
 */
#define initSDMMC(handle, instance, buswide, clockedge, powersave, 				\
						clockdiv, hwcontrol)														\
{																										\
	handle.Instance = instance;																\
	handle.Init.BusWide = buswide;															\
	handle.Init.ClockEdge = clockedge;														\
	handle.Init.ClockPowerSave = powersave;												\
	handle.Init.ClockDiv = clockdiv;															\
	handle.Init.HardwareFlowControl = hwcontrol;											\
}

/**
 * Initialize DFSDM channel with given parameters.
 */
#define initDFSDMChannel(handle, i, divider, filtertype, offset, rightshift)	\
{																										\
	handle[i].Instance = DFSDM1_Channel0 + i;												\
	handle[i].Init.OutputClock.Activation = ENABLE;										\
	handle[i].Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM;	\
	handle[i].Init.OutputClock.Divider = divider;										\
	handle[i].Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;				\
	handle[i].Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;				\
	handle[i].Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_RISING;				\
	handle[i].Init.Awd.FilterOrder = filtertype;											\
	handle[i].Init.Offset = offset;															\
	handle[i].Init.RightBitShift = rightshift;											\
}

/**
 * Initialize DFSDM filter with given parameters.
 */
#define initDFSDMFilter(handle, i, sincorder, oversampling)							\
{																										\
	handle[i].Instance = DFSDM1_Filter0 + i;												\
	handle[i].Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;					\
	handle[i].Init.RegularParam.FastMode = ENABLE;										\
	handle[i].Init.RegularParam.DmaMode = DISABLE;										\
	handle[i].Init.FilterParam.SincOrder = sincorder;									\
	handle[i].Init.FilterParam.Oversampling = oversampling;							\
	handle[i].Init.FilterParam.IntOversampling = 1;										\
}

/*****************************************************************************/
/*****************************************************************************/

/* Function Prototypes */

extern void configOscClk(void);
extern void configDebugPort(void);
extern void configIMUSensor(void);
extern void configMicSensors(void);
extern void configGPSModule(void);
extern void configSDCard(void);
extern void configLoRaModule(void);
extern void configServoMotors(void);
extern void configLEDs(void);
extern void configWatchdog(void);

extern void taskMicSensorNorth(void *);
extern void taskMicSensorEast(void *);
extern void taskMicSensorSouth(void *);
extern void taskMicSensorWest(void *);
extern void taskGPSModule(void *);
extern void taskIMUSensor(void *);
extern void taskSDCard(void *);
extern void taskLoRaModule(void *);
extern void taskSystemCheck(void *);
extern void taskServoMotors(void *);
extern void taskLEDs(void *);
extern void taskWatchdog(void *);

extern void SysTick_Handler(void);
extern void xPortSysTickHandler(void);
extern void vApplicationIdleHook(void);

extern void __parse_nmea_sentences(uint8_t *, PayloadData *);
extern void __write_to_imu_reg(uint8_t, uint8_t);
extern uint8_t __read_reg_from_imu(uint8_t);
extern void __read_accel_from_imu(PayloadData *);
extern void __read_gyro_from_imu(PayloadData *);
extern void __read_temp_from_imu(PayloadData *);
extern void __get_sd_card_info(void);

#endif /* FIRMWARE_H */
