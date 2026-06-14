/**
 ******************************************************************************
 * @file 	peripheral.h
 * @author 	Can Gulmez
 * @brief 	Peripheral headers of the firmware.
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

#include "stm32h7xx_hal.h"

/* Peripheral pin definitions */

#define DEBUG_PIN_TX					GPIO_PIN_0	/* GPIOA - UART4 */
#define DEBUG_PIN_RX					GPIO_PIN_1	/* GPIOA - UART4 */

#define MIC_PIN_DATAIN0					GPIO_PIN_1	/* GPIOC - DFSDM1 */ 
#define MIC_PIN_DATAIN1					GPIO_PIN_1	/* GPIOB - DFSDM1 */
#define MIC_PIN_DATAIN2					GPIO_PIN_5	/* GPIOC - DFSDM1 */
#define MIC_PIN_DATAIN3					GPIO_PIN_7	/* GPIOC - DFSDM1 */
#define MIC_PIN_CKOUT					GPIO_PIN_0	/* GPIOB - DFSDM1 */

#define IMU_PIN_NSS						GPIO_PIN_4	/* GPIOA - Output */
#define IMU_PIN_SCK						GPIO_PIN_5	/* GPIOA - SPI1 */
#define IMU_PIN_MISO					GPIO_PIN_6	/* GPIOA - SPI1 */
#define IMU_PIN_MOSI					GPIO_PIN_7	/* GPIOA - SPI1 */
#define IMU_PIN_INT1					GPIO_PIN_2	/* GPIOB - Input */
#define IMU_PIN_INT2					GPIO_PIN_3	/* GPIOA - Input */

#define IMU_REG_FUNC_CFG_ACCESS			0x01
#define IMU_REG_SENSOR_SYNC_TIME		0x04
#define IMU_REG_SENSOR_SYNC_RES			0x05
#define IMU_REG_FIFO_CTRL1				0x06
#define IMU_REG_FIFO_CTRL2				0x07
#define IMU_REG_FIFO_CTRL3				0x08
#define IMU_REG_FIFO_CTRL4				0x09
#define IMU_REG_FIFO_CTRL5				0x0A
#define IMU_REG_DYDY_PULSE_CFG_G		0x0B
#define IMU_REG_INT1_CTRL				0x0D
#define IMU_REG_INT2_CTRL				0x0E
#define IMU_REG_WHO_AM_I				0x0F
#define IMU_REG_CTRL1_XL				0x10
#define IMU_REG_CTRL2_G					0x11
#define IMU_REG_CTRL3_C					0x12
#define IMU_REG_CTRL4_C					0x13
#define IMU_REG_CTRL5_C					0x14
#define IMU_REG_CTRL6_C					0x15
#define IMU_REG_CTRL7_G					0x16
#define IMU_REG_CTRL8_XL				0x17
#define IMU_REG_CTRL9_XL				0x18
#define IMU_REG_CTRL10_C				0x19
#define IMU_REG_MASTER_CONFIG			0x1A
#define IMU_REG_WAKE_UP_SRC				0x1B
#define IMU_REG_TAP_SRC					0x1C
#define IMU_REG_D6D_SRC					0x1D
#define IMU_REG_STATUS_REG				0x1E	
#define IMU_REG_OUT_TEMP_L				0x20	
#define IMU_REG_OUT_TEMP_H				0x21
#define IMU_REG_OUTX_L_G				0x22
#define IMU_REG_OUTX_H_G				0x23
#define IMU_REG_OUTY_L_G				0x24
#define IMU_REG_OUTY_H_G				0x25
#define IMU_REG_OUTZ_L_G				0x26
#define IMU_REG_OUTZ_H_G				0x27
#define IMU_REG_OUTX_L_XL				0x28
#define IMU_REG_OUTX_H_XL				0x29
#define IMU_REG_OUTY_L_XL				0x2A
#define IMU_REG_OUTY_H_XL				0x2B
#define IMU_REG_OUTZ_L_XL				0x2C
#define IMU_REG_OUTZ_H_XL				0x2D
#define IMU_REG_SENSORHUB1_REG			0x2E
#define IMU_REG_SENSORHUB2_REG			0x2F
#define IMU_REG_SENSORHUB3_REG			0x30
#define IMU_REG_SENSORHUB4_REG			0x31
#define IMU_REG_SENSORHUB5_REG			0x32
#define IMU_REG_SENSORHUB6_REG			0x33
#define IMU_REG_SENSORHUB7_REG			0x34
#define IMU_REG_SENSORHUB8_REG			0x35
#define IMU_REG_SENSORHUB9_REG			0x36
#define IMU_REG_SENSORHUB10_REG			0x37
#define IMU_REG_SENSORHUB11_REG			0x38
#define IMU_REG_SENSORHUB12_REG			0x39
#define IMU_REG_FIFO_STATUS1			0x3A
#define IMU_REG_FIFO_STATUS2			0x3B
#define IMU_REG_FIFO_STATUS3			0x3C
#define IMU_REG_FIFO_STATUS4			0x3D
#define IMU_REG_FIFO_DATA_OUT_L			0x3E
#define IMU_REG_FIFO_DATA_OUT_H			0x3F
#define IMU_REG_TIMESTAMP0_REG			0x40
#define IMU_REG_TIMESTAMP1_REG			0x41
#define IMU_REG_TIMESTAMP2_REG			0x42
#define IMU_REG_STEP_TIMESTAMP_L		0x49
#define IMU_REG_STEP_TIMESTAMP_H		0x4A
#define IMU_REG_STEP_COUNTER_L			0x4B
#define IMU_REG_STEP_COUNTER_H			0x4C
#define IMU_REG_SENSORHUB13_REG			0x4D
#define IMU_REG_SENSORHUB14_REG			0x4E
#define IMU_REG_SENSORHUB15_REG			0x4F
#define IMU_REG_SENSORHUB16_REG			0x50
#define IMU_REG_SENSORHUB17_REG			0x51
#define IMU_REG_SENSORHUB18_REG			0x52
#define IMU_REG_FUNC_SRC1				0x53
#define IMU_REG_FUNC_SRC2				0x54
#define IMU_REG_WRIST_TILT_IA			0x55
#define IMU_REG_TAP_CFG					0x58
#define IMU_REG_TAP_THS_6D				0x59
#define IMU_REG_INT_DUR2				0x5A
#define IMU_REG_WAKE_UP_THS				0x5B
#define IMU_REG_WAKE_UP_DUR				0x5C
#define IMU_REG_FREE_FALL				0x5D
#define IMU_REG_MD1_CFG					0x5E
#define IMU_REG_MD2_CFG					0x5F
#define IMU_REG_MASTER_CMD_CODE			0x60
#define IMU_REG_SENS_SYNC_SPI_ERROR		0x61
#define IMU_REG_OUT_MAG_RAW_X_L			0x66
#define IMU_REG_OUT_MAG_RAW_X_H			0x67
#define IMU_REG_OUT_MAG_RAW_Y_L			0x68
#define IMU_REG_OUT_MAG_RAW_Y_H			0x69
#define IMU_REG_OUT_MAG_RAW_Z_L			0x6A
#define IMU_REG_OUT_MAG_RAW_Z_H			0x6B
#define IMU_REG_X_OFS_USR				0x73
#define IMU_REG_Y_OFS_USR				0x74
#define IMU_REG_Z_OFS_USR				0x75

#define IMU_NSS_LOW()					(HAL_GPIO_WritePin(GPIOA, IMU_PIN_NSS, RESET))
#define IMU_NSS_HIGH()					(HAL_GPIO_WritePin(GPIOA, IMU_PIN_NSS, SET))
