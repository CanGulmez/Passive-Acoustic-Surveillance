/**
 ******************************************************************************
 * @file 	utility.c
 * @author 	Can Gulmez
 * @brief 	Common utilities of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can Gulmez.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 *****************************************************************************
 */

#include "main.h"

/**
 * Write the data to IMU sensor register.
 */
void writeRegToIMU(uint8_t reg, uint8_t data)
{
	HAL_StatusTypeDef status;
	const uint8_t packet[2] = { reg | 0x00, data };	/* address and then data */

	IMU_NSS_LOW();
	/* Pull the NSS to low to start the transmission and then send the packet. */
	status = HAL_SPI_Transmit(&hspi1, packet, 2, HAL_MAX_DELAY);
	if (status != HAL_OK)
		printError(status, "Failed to write the data to the IMU register!");

	/* Pull the NSS to high to stop the transmission. */
	IMU_NSS_HIGH();
}

/**
 * Read the data from IMU sensor register.
 */
uint8_t readRegFromIMU(uint8_t reg)
{
	HAL_StatusTypeDef status;
	uint8_t txPacket[2] = {reg | 0x80, 0x00};	/* address and then data */
	uint8_t rxPacket[2] = {0};

	/* Pull the NNS to low to start the transmission. */
	IMU_NSS_LOW();
	
	status = HAL_SPI_TransmitReceive(&hspi1, txPacket, rxPacket, 2, HAL_MAX_DELAY);
	if (status != HAL_OK)
		printError(status, "Failed to read the data from the IMU register!");

	/* Pull the NSS to high to stop the transmission. */
	IMU_NSS_HIGH();

	return rxPacket[1];
}

/**
 * Read the acceleromenter data from IMU sensor.
 */
void readAccelFromIMU(PayloadData *payloadData)
{
	int i;
	uint8_t data[6];

	/* Read the associated registers in order. */
	for (i = 0; i < 6; i++)
	{
		data[i] = readRegFromIMU(IMU_REG_OUTX_L_XL + i);
	}
	/* Combine bytes (little endien) then convert to m/s^2. */
	payloadData->imuAccel[0] = ((int16_t)((data[1] << 8) | data[0])) * 0.004787f;
	payloadData->imuAccel[1] = ((int16_t)((data[3] << 8) | data[2])) * 0.004787f;
	payloadData->imuAccel[2] = ((int16_t)((data[5] << 8) | data[4])) * 0.004787f;
}

/**
 * Read the gyroscope data from IMU sensor.
 */
void readGyroFromIMU(PayloadData *payloadData)
{
	int i;
	uint8_t data[6];
	
	/* Read the associated registers in order. */
	for (i = 0; i < 6; i++)
	{
		data[i] = readRegFromIMU(IMU_REG_OUTX_L_G + i);
	}
	/* Combine bytes (little endian) then convert to dps. */
	payloadData->imuGyro[0] = ((int16_t)((data[1] << 8) | data[0])) * 0.07f;
	payloadData->imuGyro[1] = ((int16_t)((data[3] << 8) | data[2])) * 0.07f;
	payloadData->imuGyro[2] = ((int16_t)((data[5] << 8) | data[4])) * 0.07f;
}

/**
 * Read the temperature data from IMU sensor.
 */
void readTempFromIMU(PayloadData *payloadData)
{
	uint8_t low, high;

	low = readRegFromIMU(IMU_REG_OUT_TEMP_L);
	high = readRegFromIMU(IMU_REG_OUT_TEMP_H);

	payloadData->imuTemp = (((high << 8) | low) / 256.0f) + 25.0f;
}
 