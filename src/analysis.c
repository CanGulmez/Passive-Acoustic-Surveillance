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

DspTime sigSamples[MIC_COUNT] = {0};
DspTime sigBeamformed = {0};
guint sigVolumest = 1;

/**
 * Convert the payload mic data to 'DspTime' objects.
 */
void prepare_samples(void)
{
	int i;

	/* Convert the payload data to 'DspTime' objects. */
	for (i = 0; i < MIC_COUNT; i++)		/* the sample length */
	{
		sigSamples[i].length = DATA_SIZE;
	}
	for (i = 0; i < DATA_SIZE; i++)		/* the filter 0 samples */
	{
		sigSamples[0].data[i] = (double) payloadData.micFilter0[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)		/* the filter 1 samples */
	{
		sigSamples[1].data[i] = (double) payloadData.micFilter1[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)		/* the filter 2 samples */
	{
		sigSamples[2].data[i] = (double) payloadData.micFilter2[i];	
	}
	for (i = 0; i < DATA_SIZE; i++)		/* the filter 3 samples */
	{
		sigSamples[3].data[i] = (double) payloadData.micFilter3[i];	
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
		dsp_transform_fft(&sigSamples[i], &outputs[i]);
		outputs[i].length = (int) (outputs[i].length / 2);
	}
	/* Find the maximum frequencies and corresponding bins. */
	for (i = 0; i < MIC_COUNT; i++)
	{
		dsp_freq_magnitude(&outputs[i], &magnitudes[i]);
		magnitudes[i].data[0] = 0.0;	/* pass the DC bias */
		indexes[i] = dsp_time_argmax(&magnitudes[i]);
		frequencies[i] = (MIC_SAMPLING_FREQ / DATA_SIZE) * indexes[i];
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
DspTime do_beamforming(double arrival)
{
	int i;
	DspBeamform beamform;
	DspTime sample;

	beamform.mics = MIC_COUNT;	
	beamform.fs = MIC_SAMPLING_FREQ;
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
		__ui_action_row_update(micSignalRows[i], buffer[i]);
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
	sigVolumest = select_sector();

	/* Make sure the amplitude of signal fits into the frame. */
	dsp_time_scale(&sigBeamformed, 128.0, &sigBeamformed);

	/* Make the signal analysis. */
	make_signal_analysis(&sigBeamformed, arrival);
	print_log("completed the signal analysis operations");

	/* Lastly, redraw the cartesian and polar plots. */
	gtk_widget_queue_draw(micCarPlot);
	gtk_widget_queue_draw(micPolarPlot);
	print_log("requested the microphone plot redraws");
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
		payloadData.imuAccel[0], payloadData.imuAccel[1], payloadData.imuAccel[2]
	);
	__ui_action_row_update(navSensorRows[1], "Running");
	__ui_action_row_update(navSensorRows[2], buffer);

	/* Update the gyroscope output. */
	snprintf(
		buffer, BUFFER_SIZE, "[%.2f, %.2f, %.2f]", 
		payloadData.imuGyro[0], payloadData.imuGyro[1], payloadData.imuGyro[2]
	);	
	__ui_action_row_update(navSensorRows[3], "Running");
	__ui_action_row_update(navSensorRows[4], buffer);

	/* Update the temperature output. */
	snprintf(buffer, BUFFER_SIZE, "%.3f", payloadData.imuTemp);	
	__ui_action_row_update(navSensorRows[7], buffer);
	print_log("updated the navigation data recently");

	/* Select the direction and rotation for plot. */
	navAccel = accel_direction();
	navGyro = gyro_rotation();

	/* Request redraw for navigation plot. */
	gtk_widget_queue_draw(navPlotArea);
	print_log("requested the navigtion plot redraw");
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
	// __ui_action_row_update(gpsModuleRows[1], payloadData.gpsUTCTime);
	// __ui_action_row_update(gpsModuleRows[2], payloadData.gpsLatitude);
	// __ui_action_row_update(gpsModuleRows[3], payloadData.gpsLongitude);
	// __ui_action_row_update(gpsModuleRows[4], payloadData.gpsQuality);
	// __ui_action_row_update(gpsModuleRows[5], payloadData.gpsNumSat);
	// __ui_action_row_update(gpsModuleRows[6], payloadData.gpsAltitude);
	// __ui_action_row_update(gpsModuleRows[7], payloadData.gpsStatus);
	// __ui_action_row_update(gpsModuleRows[8], payloadData.gpsSpeed);
	// __ui_action_row_update(gpsModuleRows[9], payloadData.gpsCourse);
	// __ui_action_row_update(gpsModuleRows[10], payloadData.gpsDate);
	print_log("updated the GPS map data recently");

	/* Update the GPS map. */
	// latitude = atof(payloadData.gpsLatitude);
	// longitude = atof(payloadData.gpsLongitude);
	latitude = GPS_INIT_LAT;
	longitude = GPS_INIT_LONG;
	shumate_map_center_on(gpsMap, latitude, longitude + i * 0.01);
	gps_map_area_markers(gpsMarkerLayer, latitude, longitude + i * 0.01);
	print_log("marked the last position on the map");

	i++;
}
 