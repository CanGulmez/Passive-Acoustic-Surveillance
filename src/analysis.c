/**
 ******************************************************************************
 * @file 	analysis.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Payload analysis operations of passive acoustic surveillance.
 * 
 ******************************************************************************
 * @attention 
 * 
 * Copyright (c) 2026 Ahmet Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "./main.h"

/* Global and Shared Variables */

DspTime sigSamples[MIC_COUNT] = {0};
DspTime sigBeamformed = {0};
guint sigVolumest = 1;

/**
 * Convert the payload mic data to 'DspTime' objects.
 */
void convert_payload_to_sample(void)
{
	int i;

	/* Convert the payload data to 'DspTime' objects. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		sigSamples[i].length = DATA_SIZE;
	}
	for (i = 0; i < DATA_SIZE; i++)
	{
		sigSamples[0].data[i] = (double) payloadData.micNorth[i];
		sigSamples[1].data[i] = (double) payloadData.micNorthEast[i];
		sigSamples[2].data[i] = (double) payloadData.micEast[i];
		sigSamples[3].data[i] = (double) payloadData.micSouthEast[i];
		sigSamples[4].data[i] = (double) payloadData.micSouth[i];
		sigSamples[5].data[i] = (double) payloadData.micSouthWest[i];
		sigSamples[6].data[i] = (double) payloadData.micWest[i];
		sigSamples[7].data[i] = (double) payloadData.micNorthWest[i];
	}
}

/**
 * Find the dominant frequency around the mic outputs.
 */
double find_dominant_freq(void)
{
	int i;
	DspFreq outputs[MIC_COUNT];
	DspTime magnitudes[MIC_COUNT];
	int indexes[MIC_COUNT];
	double frequencies[MIC_COUNT];
	double max_freq = 0;

	/* Convert the time domain signals into frequency domain. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		dsp_transform_dft(&sigSamples[i], &outputs[i]);
		outputs[i].length = (int) (outputs[i].length / 2);
	}
	/* Find the maximum frequencies and corresponding bins. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		dsp_freq_magnitude(&outputs[i], &magnitudes[i]);
		magnitudes[i].data[0] = 0.0;	/* pass the DC bias */
		indexes[i] = dsp_time_argmax(&magnitudes[i]);
		frequencies[i] = (MIC_SAMPLE_FREQ / DATA_SIZE) * indexes[i];
	}
	/* Compare the frequencies to find the maximum. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		if (frequencies[i] > max_freq)
		{
			max_freq = frequencies[i];
		}
	}
	return max_freq;
}

/**
 * Calculate the arrival of angle from coming signals.
 */
int calculate_arrival(double freq)
{
	int i;
	DspArrival arrival;

	arrival.mics = MIC_COUNT;
	arrival.freq = freq;
	arrival.radius = MIC_RADIUS;
	arrival.sources = 1;
	for (i = 0; i < MIC_COUNT; i++)
	{
		arrival.samples[i] = &sigSamples[i];	/* samples itself */
	}
	return dsp_arrival_music(&arrival);
}

/**
 * Make the delay-and-sum beamforming.
 */
DspTime do_beamforming(double freq, double arrival)
{
	int i;
	DspBeamform beamform;
	DspTime sample;

	beamform.mics = MIC_COUNT;	
	beamform.freq = freq;	
	beamform.radius = MIC_RADIUS;	
	beamform.theta = arrival;
	for (i = 0; i < MIC_COUNT; i++)	
	{
		beamform.samples[i] = &sigSamples[i];	/* samples itself */
	}
	dsp_beamform_delay_sum(&beamform, &sample);

	return sample;
}

/**
 * Make the other signal analysis to update `MicSignal` struct.
 */
void make_signal_analysis(const DspTime *beamformed, int arrival)
{
	int i;
	char buffer[MIC_SIGNAL_NUM][BUFFER_SIZE];

	/* Make the signal analysis one by one. */
	snprintf(buffer[0], BUFFER_SIZE, "%.4f", dsp_time_max(beamformed));	/* maximum */
	snprintf(buffer[1], BUFFER_SIZE, "%.4f", dsp_time_min(beamformed));	/* minimum */
	snprintf(buffer[2], BUFFER_SIZE, "%.4f", dsp_time_mean(beamformed));	/* mean */
	snprintf(buffer[3], BUFFER_SIZE, "%.4f", dsp_time_stddev(beamformed)); /* standard deviation */
	snprintf(buffer[4], BUFFER_SIZE, "%.4f", dsp_time_energy(beamformed));	/* energy */
	snprintf(buffer[5], BUFFER_SIZE, "%.4f", dsp_time_rms(beamformed));	/* RMS */
	snprintf(buffer[6], BUFFER_SIZE, "%.4f", dsp_time_power(beamformed));	/* power */
	snprintf(buffer[7], BUFFER_SIZE, "%.4f", dsp_time_crest_factor(beamformed));	/* crest factor */
	snprintf(buffer[8], BUFFER_SIZE, "%.4f", dsp_time_skewness(beamformed));	/* skewness */
	snprintf(buffer[9], BUFFER_SIZE, "%.4f", dsp_time_kurtosis(beamformed));	/* kurtosis */
	snprintf(buffer[10], BUFFER_SIZE, "%.4f", dsp_time_variance(beamformed));	/* variance */
	snprintf(buffer[11], BUFFER_SIZE, "%d", arrival);	/* arrival of angle */

	/* Update the signal analysis rows. */
	for (i = 0; i < 12; i++)
	{
		__generic_action_row_update(micSignalRows[i], buffer[i]);
	}
}

/**
 * Return the sector that has much more intensity.
 */
int select_sector(void)
{
	int i;
	double means[MIC_COUNT], biggest;

	/* Get the mean of sensor signals. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		means[i] = dsp_time_mean(&sigSamples[i]);
	}
	/* Find the biggest mean. */
	biggest = means[0];
	for (i = 0; i < MIC_COUNT; i++)
	{
		if (means[i] > biggest)
		{
			biggest = means[i];
		}
	}
	for (i = 0; i < MIC_COUNT; i++)
	{
		if (means[i] == biggest)
		{
			return i + 1;
		}
	}
	/* Never should come here. */
	return -1;
}

/**
 * Select the accelerometer direction to be drawed.
 */
NavAccel select_accel_direction(void)
{
	if ((payloadData.imuAccelX > payloadData.imuAccelY) &&
		 (payloadData.imuAccelX > payloadData.imuAccelZ))
	{
		return NAV_ACCEL_X_PLUS;
	}
	else if ((payloadData.imuAccelY > payloadData.imuAccelX) &&
		 		(payloadData.imuAccelY > payloadData.imuAccelZ))
	{
		return NAV_ACCEL_Y_PLUS;
	}
	else if ((payloadData.imuAccelZ > payloadData.imuAccelX) &&
		 		(payloadData.imuAccelZ > payloadData.imuAccelY))
	{
		return NAV_ACCEL_Z_PLUS;
	}
	else if ((payloadData.imuAccelX < payloadData.imuAccelY) &&
		 		(payloadData.imuAccelX < payloadData.imuAccelZ))
	{
		return NAV_ACCEL_X_MINUS;
	}
	else if ((payloadData.imuAccelY < payloadData.imuAccelX) &&
		 		(payloadData.imuAccelY < payloadData.imuAccelZ))
	{
		return NAV_ACCEL_Y_MINUS;
	}
	else if ((payloadData.imuAccelZ < payloadData.imuAccelX) &&
		 		(payloadData.imuAccelZ < payloadData.imuAccelY))
	{
		return NAV_ACCEL_Z_MINUS;
	}
	return NAV_ACCEL_UNDEF;
}

/**
 * Select the gyroscope rotation to be drawed.
 */
NavGyro select_gyro_rotation(void)
{
	if ((payloadData.imuGyroX > payloadData.imuGyroY) &&
		 (payloadData.imuGyroX > payloadData.imuGyroZ))
	{
		return NAV_GYRO_X_PLUS;
	}
	else if ((payloadData.imuGyroY > payloadData.imuGyroX) &&
		 		(payloadData.imuGyroY > payloadData.imuGyroZ))
	{
		return NAV_GYRO_Y_PLUS;
	}
	else if ((payloadData.imuGyroZ > payloadData.imuGyroX) &&
		 		(payloadData.imuGyroZ > payloadData.imuGyroY))
	{
		return NAV_GYRO_Z_PLUS;
	}
	else if ((payloadData.imuGyroX < payloadData.imuGyroY) &&
		 		(payloadData.imuGyroX < payloadData.imuGyroZ))
	{
		return NAV_GYRO_X_MINUS;
	}
	else if ((payloadData.imuGyroY < payloadData.imuGyroX) &&
		 		(payloadData.imuGyroY < payloadData.imuGyroZ))
	{
		return NAV_GYRO_Y_MINUS;
	}
	else if ((payloadData.imuGyroZ < payloadData.imuGyroX) &&
		 		(payloadData.imuGyroZ < payloadData.imuGyroY))
	{
		return NAV_GYRO_Z_MINUS;
	}
	return NAV_GYRO_UNDEF;
}

/**
 * Update the navigation data including IMU outputs.
 */
void update_nav_data(void)
{
	char buffer[BUFFER_SIZE];

	/* Update the sensor series and information. */
	__generic_action_row_update(navSensorRows[0], NAV_IMU_SENSOR);

	/* Update the acceloremeter output. */
	snprintf(
		buffer, BUFFER_SIZE, "[%.2f, %.2f, %.2f]", 
		payloadData.imuAccelX, payloadData.imuAccelY, payloadData.imuAccelZ
	);
	__generic_action_row_update(navSensorRows[1], "Running");
	__generic_action_row_update(navSensorRows[2], buffer);

	/* Update the gyroscope output. */
	snprintf(
		buffer, BUFFER_SIZE, "[%.2f, %.2f, %.2f]", 
		payloadData.imuGyroX, payloadData.imuGyroY, payloadData.imuGyroZ
	);	
	__generic_action_row_update(navSensorRows[3], "Running");
	__generic_action_row_update(navSensorRows[4], buffer);

	/* Update the temperature output. */
	snprintf(
		buffer, BUFFER_SIZE, "%.3f", payloadData.imuTemp
	);	
	__generic_action_row_update(navSensorRows[7], buffer);
}

/**
 * Update the GPS data including LoRa outputs.
 */
void update_gps_data(void)
{
	/* Update the GPS module information. */
	__generic_action_row_update(gpsModuleRows[0], GPS_MODULE);

	/* Update the GPS module outputs. */
	__generic_action_row_update(gpsModuleRows[1], payloadData.gpsUTCTime);
	__generic_action_row_update(gpsModuleRows[2], payloadData.gpsLatitude);
	__generic_action_row_update(gpsModuleRows[3], payloadData.gpsLongitude);
	__generic_action_row_update(gpsModuleRows[4], payloadData.gpsQuality);
	__generic_action_row_update(gpsModuleRows[5], payloadData.gpsNumSat);
	__generic_action_row_update(gpsModuleRows[6], payloadData.gpsAltitude);
	__generic_action_row_update(gpsModuleRows[7], payloadData.gpsStatus);
	__generic_action_row_update(gpsModuleRows[8], payloadData.gpsSpeed);
	__generic_action_row_update(gpsModuleRows[9], payloadData.gpsCourse);
	__generic_action_row_update(gpsModuleRows[10], payloadData.gpsDate);
}
 
