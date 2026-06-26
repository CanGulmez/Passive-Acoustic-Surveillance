/**
 ******************************************************************************
 * @file 	analysis.c
 * @author 	Can Gulmez
 * @brief 	Payload analysis operations of passive acoustic surveillance.
 * 
 ******************************************************************************
 * @attention 
 * 
 * Copyright (c) 2026 Can Gulmez.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "main.h"

/* Global and Shared Variables */

DspTime sigSamples[MIC_COUNT] = {0};	/* MK1-MK8 */
DspTime sigBeamformed = {0};
mic_t sigVolumest = 1;

/**
 * Convert the payload mic data to 'DspTime' objects.
 */
void prepare_samples(void)
{
	int i;

	/* Convert the payload data to 'DspTime' objects. */
	for (i = 0; i < MIC_COUNT; i++)			/* sample length */
	{
		sigSamples[i].length = DATA_SIZE;
	}
	for (i = 0; i < DATA_SIZE; i++)			/* MK1 */
	{
		sigSamples[0].data[i] = 0.0;
	}
	for (i = 0; i < DATA_SIZE; i++)			/* filter 0 - MK2 */
	{
		sigSamples[1].data[i] = (double) payloadData.micFilter0[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)			/* MK3 */
	{
		sigSamples[2].data[i] = 0.0;
	}
	for (i = 0; i < DATA_SIZE; i++)			/* MK4 */
	{
		sigSamples[3].data[i] = 0.0;
	}
	for (i = 0; i < DATA_SIZE; i++)			/* filter 3 - MK5 */
	{
		sigSamples[4].data[i] = (double) payloadData.micFilter3[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)			/* filter 2 - MK6 */
	{
		sigSamples[5].data[i] = (double) payloadData.micFilter2[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)			/* filter 1 - MK7 */
	{
		sigSamples[6].data[i] = (double) payloadData.micFilter1[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)			/* MK8 */
	{
		sigSamples[7].data[i] = 0.0;
	}
}

/**
 * Find the dominant frequency around the mic outputs.
 */
double find_dominant_freq(void)
{
	int i;
	DspFreq outputs[MIC_COUNT] = {0};
	DspTime magnitudes[MIC_COUNT] = {0};
	int indexes[MIC_COUNT] = {0};
	double frequencies[MIC_COUNT] = {0};
	double max_freq = 0;

	printf("MK2: %.4f, %.4f, %.4f\n", sigSamples[1].data[0], 
		sigSamples[1].data[100], sigSamples[1].data[250]);
	// printf("MK5: %.4f, %.4f, %.4f, %.4f\n", sigSamples[4].data[0], 
	// 	sigSamples[4].data[100], sigSamples[4].data[250], sigSamples[4].data[600]);
	// printf("MK6: %.4f, %.4f, %.4f, %.4f\n", sigSamples[5].data[0], 
	// 	sigSamples[5].data[100], sigSamples[5].data[250], sigSamples[5].data[600]);
	// printf("MK7: %.4f, %.4f, %.4f, %.4f\n", sigSamples[6].data[0], 
	// 	sigSamples[6].data[100], sigSamples[6].data[250], sigSamples[6].data[600]);

	/* Convert the time domain samples into frequency domain and then 
	   calculate the magintudes to find the maximum frequencies and 
	   corresponding bins. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		dsp_transform_dft(&sigSamples[i], &outputs[i]);
		// dsp_freq_magnitude(&outputs[i], &magnitudes[i]);
		// magnitudes[i].data[0] = 0.0;	/* pass the DC bias */
		// indexes[i] = dsp_time_argmax(&magnitudes[i]);
		// frequencies[i] = (MIC_SAMPLING_FREQ / DATA_SIZE) * indexes[i];
	}
	printf("MK2: [%.2f, %.2f], [%.2f, %.2f], [%.2f, %.2f]\n", 
		outputs[1].data[0][0], outputs[1].data[0][1],
		outputs[1].data[100][0], outputs[1].data[100][1],
		outputs[1].data[250][0], outputs[1].data[250][1]);
	// printf("MK5: %.2f, %.2f, %.2f\n", outputs[4].data[0][0], 
	// 	outputs[4].data[100][0], outputs[4].data[250][0], outputs[4].data[600][0]);
	// printf("MK6: %.2f, %.2f, %.2f\n", outputs[5].data[0][0], 
	// 	outputs[5].data[100][0], outputs[5].data[250][0], outputs[5].data[600][0]);
	// printf("MK7: %.2f, %.2f, %.2f\n", outputs[6].data[0][0], 
	// 	outputs[6].data[100][0], outputs[6].data[250][0], outputs[6].data[600][0]);
		
	// printf("\n");
	// printf("frequencies: ");
	// printf("MK2: %.2f, MK5: %.2f, MK6: %.2f, MK7: %.2f", frequencies[1], frequencies[4], frequencies[5], frequencies[6]);
	// printf("\n");
	/* Compare the frequencies to find the maximum one. */
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
 * Make the delay-and-sum beamforming to enhance the signals.
 */
DspTime do_beamforming(int arrival)
{
	int i;
	DspBeamform beamform;
	DspTime sample;

	beamform.mics = MIC_COUNT;	
	beamform.fs = MIC_SAMPLING_FREQ;
	beamform.radius = MIC_RADIUS;	
	beamform.theta = (double) arrival;
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
		__ui_action_row_update(micSignalRows[i], buffer[i]);
	}
}

/**
 * Return the sector that has much more intensity.
 */
mic_t select_sector(int arrival)
{
	int i;
	mic_t sector = 1;

	if ((arrival <= 22) && (arrival >= 337))
	{
		sector = 1;		/* MK1 */
	}
	else if ((arrival <= 67) && (arrival >= 23))
	{
		sector = 2;		/* MK2 */
	}
	else if ((arrival <= 112) && (arrival >= 68))
	{
		sector = 3;		/* MK3 */
	}
	else if ((arrival <= 157) && (arrival >= 113))
	{
		sector = 4;		/* MK4 */
	}
	else if ((arrival <= 202) && (arrival >= 158))
	{
		sector = 5;		/* MK5 */
	}
	else if ((arrival <= 247) && (arrival >= 203))
	{
		sector = 6;		/* MK6 */
	}
	else if ((arrival <= 292) && (arrival >= 248))
	{
		sector = 7;		/* MK7 */
	}
	else if ((arrival <= 336) && (arrival >= 293))
	{
		sector = 8;		/* MK8 */
	}	
	return sector;
}

/**
 * Select the accelerometer direction to be drawed.
 */
NavAccel accel_direction(void)
{
	double accelX = payloadData.imuAccel[0];
	double accelY = payloadData.imuAccel[1];
	double accelZ = payloadData.imuAccel[2] - NAV_FLAT_GRAVITY;

	if ((accelX > accelY) && (accelX > accelZ))
	{
		return NAV_ACCEL_X_PLUS;
	}
	else if ((accelY > accelX) && (accelY > accelZ))
	{
		return NAV_ACCEL_Y_PLUS;
	}
	else if ((accelZ > accelX) && (accelZ > accelY))
	{
		return NAV_ACCEL_Z_PLUS;
	}
	else if ((accelX < accelY) && (accelX < accelZ))
	{
		return NAV_ACCEL_X_MINUS;
	}
	else if ((accelY < accelX) && (accelY < accelZ))
	{
		return NAV_ACCEL_Y_MINUS;
	}
	else if ((accelZ < accelX) && (accelZ < accelY))
	{
		return NAV_ACCEL_Z_MINUS;
	}
	return NAV_ACCEL_UNDEF;
}

/**
 * Select the gyroscope rotation to be drawed.
 */
NavGyro gyro_rotation(void)
{
	double gyroX = payloadData.imuGyro[0];
	double gyroY = payloadData.imuGyro[1];
	double gyroZ = payloadData.imuGyro[2];

	if ((gyroX > gyroY) && (gyroX > gyroZ))
	{
		return NAV_GYRO_X_PLUS;
	}
	else if ((gyroY > gyroX) && (gyroY > gyroZ))
	{
		return NAV_GYRO_Y_PLUS;
	}
	else if ((gyroZ > gyroX) && (gyroZ > gyroY))
	{
		return NAV_GYRO_Z_PLUS;
	}
	else if ((gyroX < gyroY) && (gyroX < gyroZ))
	{
		return NAV_GYRO_X_MINUS;
	}
	else if ((gyroY < gyroX) && (gyroY < gyroZ))
	{
		return NAV_GYRO_Y_MINUS;
	}
	else if ((gyroZ < gyroX) && (gyroZ < gyroY))
	{
		return NAV_GYRO_Z_MINUS;
	}
	return NAV_GYRO_UNDEF;
}

/**
 * Update the four microphone data include filter 0 to 3.
 */
void update_mic_data(void)
{
	double maxFreq;
	int arrival;
	
	/* Prepare the collected data for signal analysis. */
	prepare_samples();

	/* Extract the required calculations in here. */
	maxFreq = find_dominant_freq();
	arrival = calculate_arrival(maxFreq);
	sigBeamformed = do_beamforming(arrival);
	sigVolumest = select_sector(arrival);

	/* Make sure the amplitude of signal fits into the frame. */
	dsp_time_scale(&sigBeamformed, 128.0, &sigBeamformed);

	/* Make the signal analysis. */
	make_signal_analysis(&sigBeamformed, arrival);
	// print_log("completed the signal analysis operations");

	/* Lastly, redraw the cartesian and polar plots. */
	gtk_widget_queue_draw(micCarPlot);
	gtk_widget_queue_draw(micPolarPlot);
	// print_log("requested the microphone plot redraws");
}

/**
 * Update the nagivation data.
 */
void update_nav_data(void)
{
	char buffer[BUFFER_SIZE];

	/* Update the sensor series and information. */
	__ui_action_row_update(navSensorRows[0], NAV_IMU_SENSOR);

	/* Update the acceloremeter output. */
	snprintf(
		buffer, BUFFER_SIZE, "[%.2f, %.2f, %.2f]", 
		payloadData.imuAccel[0], payloadData.imuAccel[1], 
		payloadData.imuAccel[2]
	);
	__ui_action_row_update(navSensorRows[1], "Running");
	__ui_action_row_update(navSensorRows[2], buffer);

	/* Update the gyroscope output. */
	snprintf(
		buffer, BUFFER_SIZE, "[%.2f, %.2f, %.2f]", 
		payloadData.imuGyro[0], payloadData.imuGyro[1], 
		payloadData.imuGyro[2]
	);	
	__ui_action_row_update(navSensorRows[3], "Running");
	__ui_action_row_update(navSensorRows[4], buffer);

	/* Update the temperature output. */
	snprintf(buffer, BUFFER_SIZE, "%.3f", payloadData.imuTemp);	
	__ui_action_row_update(navSensorRows[7], buffer);
	// print_log("updated the navigation data recently");

	/* Select the direction and rotation for plot. */
	navAccel = accel_direction();
	navGyro = gyro_rotation();

	/* Request redraw for navigation plot. */
	gtk_widget_queue_draw(navPlotArea);
	// print_log("requested the navigtion plot redraw");
}

/**
 * Update the GPS data including LoRa outputs.
 */
void update_gps_data(void)
{
	static int i = 0;
	double longitude, latitude;

	/* Update the GPS module information. */
	__ui_action_row_update(gpsModuleRows[0], GPS_MODULE);

	/* Update the GPS module outputs. */
	__ui_action_row_update(gpsModuleRows[1], "0.0");
	__ui_action_row_update(gpsModuleRows[2], "0.0");
	__ui_action_row_update(gpsModuleRows[3], "0.0");
	__ui_action_row_update(gpsModuleRows[4], "0.0");
	__ui_action_row_update(gpsModuleRows[5], "0.0");
	__ui_action_row_update(gpsModuleRows[6], "0.0");
	__ui_action_row_update(gpsModuleRows[7], "0.0");
	__ui_action_row_update(gpsModuleRows[8], "0.0");
	__ui_action_row_update(gpsModuleRows[9], "0.0");
	__ui_action_row_update(gpsModuleRows[10], "0.0");
	// print_log("updated the GPS map data recently");

	/* Update the GPS map. */
	// latitude = atof(payloadData.gpsLatitude);
	// longitude = atof(payloadData.gpsLongitude);
	latitude = GPS_INIT_LAT;
	longitude = GPS_INIT_LONG;
	// shumate_map_center_on(gpsMap, latitude, longitude + i * 0.01);
	// gps_map_area_markers(gpsMarkerLayer, latitude, longitude + i * 0.01);
	// print_log("marked the last position on the gps map");

	i++;
}
 