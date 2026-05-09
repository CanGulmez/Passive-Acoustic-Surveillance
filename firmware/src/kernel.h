/**
 ******************************************************************************
 * @file 	kernel.h
 * @author 	Ahmet Can GULMEZ
 * @brief 	Kernel headers of the firmware.
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

/* Task Names */

#define TASK_MIC_NAME_N				"taskMicSensorNorth"
#define TASK_MIC_NAME_E				"taskMicSensorEast"
#define TASK_MIC_NAME_S				"taskMicSensorSouth"
#define TASK_MIC_NAME_W				"taskMicSensorWest"
#define TASK_GPS_NAME				"taskGPSModule"
#define TASK_IMU_NAME				"taskIMUSensor"
#define TASK_SD_NAME					"taskSDCard"
#define TASK_SERVO_NAME				"taskServoMotors"
#define TASK_LORA_NAME				"taskLoRaModule"
#define TASK_SYSTEM_NAME			"taskSystemCheck"
#define TASK_LED_NAME				"taskLEDs"
#define TASK_WATCHDOG_NAME			"taskWatchdog"

/* Task Stacks in Words (1 Word = 4 Bytes) */

#define TASK_MIC_STACK				2048
#define TASK_GPS_STACK				2048
#define TASK_IMU_STACK				2048
#define TASK_SD_STACK				2048
#define TASK_SERVO_STACK			2048
#define TASK_LORA_STACK				2048
#define TASK_SYSTEM_STACK			2048
#define TASK_LED_STACK				2048
#define TASK_WATCHDOG_STACK		2048

/* Task Piriorities */

#define TASK_MIC_PRIORITY			3
#define TASK_IMU_PRIORITY			2
#define TASK_GPS_PRIORITY			1
#define TASK_SD_PRIORITY			1
#define TASK_LORA_PRIORITY			1
#define TASK_SYSTEM_PRIORITY		1
#define TASK_LED_PRIORITY			1
#define TASK_WATCHDOG_PRIORITY	1

/* Task Delays */
 
// #define TASK_MIC_DELAY				pdMS_TO_TICKS(1000)	/* 1 ms */
// #define TASK_GPS_DELAY				pdMS_TO_TICKS(2000)	/* 2 ms */
// #define TASK_IMU_DELAY				pdMS_TO_TICKS(2000)	/* 2 ms */
// #define TASK_LORA_DELAY				pdMS_TO_TICKS(2000)	/* 2 ms */
// #define TASK_SD_DELAY				pdMS_TO_TICKS(5000)	/* 5 ms */
