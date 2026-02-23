/**
 ******************************************************************************
 * @file 	timeouts.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Timeout utilities of passive acoustic surveillance.
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

#include "main.h"

/**
 * Set the timeout to get the device data simultenously.
 */
gboolean timeout_device_node(gpointer data)
{
	double max_freq;
	int arrival;
	int deviceFd;

	/* Read the device node to update 'payloadData' object. */
	deviceFd = GPOINTER_TO_INT(data);
	read_device_node(deviceFd);

	/* Prepare the collected data for signal analysis. */
	convert_payload_to_sample();

	/* Extract the required calculations in here. */
	max_freq = find_dominant_freq();
	arrival = calculate_arrival(max_freq);
	sigBeamformed = do_beamforming(max_freq, arrival);
	sigVolumest = select_sector();

	/* Make sure the amplitude of signal fits into the frame. */
	dsp_time_scale(&sigBeamformed, 128.0, &sigBeamformed);

	/* Make the signal analysis. */
	make_signal_analysis(&sigBeamformed, arrival);
	printLog("completed the signal analysis operations");

	/* Lastly, redraw the cartesian and polar plots. */
	gtk_widget_queue_draw(micCarPlot);
	gtk_widget_queue_draw(micPolarPlot);
	printLog("requested the microphone plot redraws");

	return G_SOURCE_CONTINUE;
}

/**
 * Set the timeout to get the Keras logs into text view.
 */
gboolean timeout_model_keras_log(gpointer data)
{
	const char *kerasLog;

	/* Get the current text view and then update it. */
	modelTextBuffer = gtk_text_view_get_buffer(
									GTK_TEXT_VIEW(modelTextView));
	kerasLog = get_keras_script_logs(MODEL_LOG_PATH);

	gtk_text_buffer_set_text(modelTextBuffer, kerasLog, -1);
	if ( is_keras_script_running(modelFitPid) )
	{
		return G_SOURCE_CONTINUE;
	}
	else
	{
		return G_SOURCE_REMOVE;
	}
}

/**
 * Set the timeout to record the sensor data into database.
 */
gboolean timeout_db_record(gpointer data)
{
	sqlite3 *db;

	db = (sqlite3 *) data;
	/* Bind the last sensor data. */
	db_bind_data(db, DATABASE_SENSOR_DATA);

	return G_SOURCE_CONTINUE;
}

/**
 * Set the timeout for navigation updates.
 */
gboolean timeout_nav_update(gpointer data)
{
	/* Update the navigation data. */
	update_nav_data();
	printLog("updated the navigation data correctly");

	/* Select the direction and rotation for plot. */
	navAccel = select_accel_direction();
	navGyro = select_gyro_rotation();

	/* Request redraw for navigation plot. */
	gtk_widget_queue_draw(navPlotArea);
	printLog("requested the navigtion plot redraw");

	return G_SOURCE_CONTINUE;
}

/**
 * Set the timeout for GPS map updates.
 */
gboolean timeout_gps_update(gpointer)
{
	static int i = 0;
	double longitude, latitude;

	/* Update the gps module data. */
	update_gps_data();
	printLog("updated the GPS map data correctly");

	/* Update the GPS map. */
	latitude = atof(payloadData.gpsLatitude);
	longitude = atof(payloadData.gpsLongitude);
	shumate_map_center_on(gpsMap, latitude, longitude + i * 0.01);
	gps_map_area_markers(gpsMarkerLayer, latitude, longitude + i * 0.01);
	printLog("marked the last position onto the map");

	i++;

	return G_SOURCE_CONTINUE;
}
 