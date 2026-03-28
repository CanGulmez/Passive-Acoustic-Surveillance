/**
 ******************************************************************************
 * @file 	utility.c
 * @author 	Can GULMEZ
 * @brief 	Common utilities of the firmware.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2025 Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 *****************************************************************************
 */

#include "main.h"

/**
 * Parse the NMEA sentences coming from GPS module.
 */
void __parse_nmea_sentences(uint8_t *buffer, PayloadData *payloadData)
{
	int length;
	uint8_t *sentence, *token;

	/* GPS Data (NMEA 0183 sentences)
	
		$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
			+ 123519 		--> UTC time (12:35:19)
			+ 4807.038,N	--> latitude 48^07.038' North
			+ 01131.000,E	--> longitude 11^31.000' East
			+ 1 				--> fix quality (1 = GPS fix)
			+ 08				--> number of sattelite used
			+ 545.4,M		--> altitude 545.4 m
	
		$GPRCM,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
			+ 123519			--> UTC time
			+ A				--> status (A = valid, V = invalid)
			+ 					--> latitude
			+ 					--> longitude
			+ 022.4			--> speed over ground (knots)
			+ 084.4			--> course over ground (degrees)
			+ 230394			--> date (23 March 1994)
	*/
	
	/* Tokenize the each sentence in buffer. */
	sentence = strtok(buffer, "\n");
	while (sentence != NULL)
	{
		if (strncmp(sentence, "$GPGGA", 6) == 0)
		{
			token = strchr(sentence, sentence[7]);

			/* Get the UTC time (123519 --> 12:35:19). */
			strncpy(payloadData->gpsUTCTime, token, 6);
			
			/* Get the latitude (4807.038 --> 48^ 07.038' N). */
			/* Get the longitude (01131.000 --> 11^ 31.000' E). */
			/* Get the fix quality (1 --> GPS fix). */
			/* Get the number of satellites used. */
			/* Get the altitude (545.4 --> 545.4 meters). */
			
		}
		else if (strncmp(sentence, "$GPRCM", 6) == 0)
		{
			token = strchr(sentence, sentence[7]);

			/* Get the status (A --> valid, V --> invalid). */
			/* Get the speed over ground (knots). */
			/* Get the course over ground (degrees). */
			/* Get the date (230394 --> 23 March 1994). */
		}
		else
		{
			continue;
		}
		/* Proceed to the next sentence. */
		strtok(NULL, "\n");
	}
}

/**
 * Write the data to IMU sensor register.
 */
void __write_to_imu_reg(uint8_t reg, uint8_t data)
{
	HAL_StatusTypeDef status;
	const uint8_t tx_data[2] = {reg | 0x00, data};	/* address then data */

	/* Pull the NNS to low to start the transmission. */
	IMU_NSS_LOW();
	status = HAL_SPI_Transmit(&hspi1, tx_data, 2, HAL_MAX_DELAY);
	if (status != HAL_OK)
		printError(status, "Failed to write to the IMU register!");

	/* Pull the NSS to high to stop the transmission. */
	IMU_NSS_HIGH();
}

/**
 * Read the IMU sensor register.
 */
uint8_t __read_reg_from_imu(uint8_t reg)
{
	HAL_StatusTypeDef status;
	const uint8_t tx_data[2] = {reg | 0x80, 0x00};	/* address then data */
	const uint8_t rx_data[2] = {0};

	/* Pull the NNS to low to start the transmission. */
	IMU_NSS_LOW();
	
	status = HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, HAL_MAX_DELAY);
	if (status != HAL_OK)
		printError(status, "Failed to read the IMU register!");

	/* Pull the NSS to high to stop the transmission. */
	IMU_NSS_HIGH();

	return rx_data[1];
}

/**
 * Read the acceleromenter from IMU sensor.
 */
void __read_accel_from_imu(PayloadData *payloadData)
{
	int i;
	uint8_t data[6];
	
	/* Read the associated registers in order. */
	for (i = 0; i < 6; i++)
	{
		data[i] = __read_reg_from_imu(IMU_REG_OUTX_L_XL + i);
	}
	/* Combine bytes (little endian) then convert to mg. */
	payloadData->imuAccelX = ((int16_t)((data[1] << 8) | data[0])) * 0.004787f;
	payloadData->imuAccelY = ((int16_t)((data[3] << 8) | data[2])) * 0.004787f;
	payloadData->imuAccelZ = ((int16_t)((data[5] << 8) | data[4])) * 0.004787f;
}

/**
 * Read the gyroscope from IMU sensor.
 */
void __read_gyro_from_imu(PayloadData *payloadData)
{
	int i;
	uint8_t data[6];
	
	/* Read the associated registers in order. */
	for (i = 0; i < 6; i++)
	{
		data[i] = __read_reg_from_imu(IMU_REG_OUTX_L_G + i);
	}
	/* Combine bytes (little endian) then convert to dps. */
	payloadData->imuGyroX = ((int16_t)((data[1] << 8) | data[0])) * 0.07f;
	payloadData->imuGyroY = ((int16_t)((data[3] << 8) | data[2])) * 0.07f;
	payloadData->imuGyroZ = ((int16_t)((data[5] << 8) | data[4])) * 0.07f;
}

/**
 * Read the temperature from IMU sensor.
 */
void __read_temp_from_imu(PayloadData *payloadData)
{
	uint8_t temp_l, temp_h;

	temp_l = __read_reg_from_imu(IMU_REG_OUT_TEMP_L);
	temp_h = __read_reg_from_imu(IMU_REG_OUT_TEMP_H);

	payloadData->imuTemp = (((temp_h << 8) | temp_l) / 256.0f) + 25.0f;
}

/**
 * Get the SD Card information.
 */
void __get_sd_card_info(void)
{
	HAL_StatusTypeDef status;
	HAL_SD_CardInfoTypeDef info;

	/* Get the card information. */
	status = HAL_SD_GetCardInfo(&hsdmmc1, &info);
	if (status != HAL_OK)
		printError(status, "Failed to get SD Card information!");

	/* Print the required card information. */
	printLog("SD Card Info:");
	printLog("	Capacity		: %lu sectors", info.BlockNbr);
	printLog("	Sector Size	: %lu bytes", info.BlockSize);
	printLog("	Totol Size	: %lu MB",
		((info.BlockNbr * info.BlockSize) / (1024 * 1024)));
	printLog("	Card Type	: %s",
		(info.CardType == CARD_SDSC) ? "SDSC" :
		(info.CardType == CARD_SDHC_SDXC) ? "SDHC/SDXC" : "UNKNOWN");
	printLog("	Speed			: %.2f Mo/s",
		(info.CardSpeed == CARD_NORMAL_SPEED) ? 12.5 :
		(info.CardSpeed == CARD_HIGH_SPEED) ? 25.0 : 0.0);
}
