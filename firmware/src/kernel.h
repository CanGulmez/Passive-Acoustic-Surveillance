/**
 ******************************************************************************
 * @file 	kernel.h
 * @author 	Can Gulmez
 * @brief 	Kernel headers of the firmware.
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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

/* RTOS task names */

#define TASK_MIC_NAME_0				"taskMicSensor0"
#define TASK_MIC_NAME_1				"taskMicSensor1"
#define TASK_MIC_NAME_2				"taskMicSensor2"
#define TASK_MIC_NAME_3				"taskMicSensor3"
#define TASK_IMU_NAME				"taskIMUSensor"
#define TASK_SERIAL_NAME			"taskTransmitter"

/* RTOS task stacks (1 word = 4 bytes) */

#define TASK_MIC_STACK				4096	/* 16 KB */
#define TASK_IMU_STACK				1024	/* 4 KB */
#define TASK_SERIAL_STACK			1024	/* 4 KB */

/* RTOS task priorities */

#define TASK_MIC_PRIORITY			1
#define TASK_IMU_PRIORITY			1
#define TASK_SERIAL_PRIORITY		2

/* RTOS task event group bits */

#define TASK_MIC_EVENT_BIT_0		(1 << 0)
#define TASK_MIC_EVENT_BIT_1		(1 << 1)
#define TASK_MIC_EVENT_BIT_2		(1 << 2)
#define TASK_MIC_EVENT_BIT_3		(1 << 3)
#define TASK_IMU_EVENT_BIT			(1 << 4)
#define TASK_SERIAL_EVENT_BIT		(1 << 5)
