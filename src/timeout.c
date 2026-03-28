/**
 ******************************************************************************
 * @file 	timeouts.c
 * @author 	Can GULMEZ
 * @brief 	Timeout utilities of passive acoustic surveillance.
 * 
 ******************************************************************************
 * @attention 
 * 
 * Copyright (c) 2026 Can GULMEZ.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "./main.h"

/**
 * Set the timeout to get the device data simultenously.
 */
gboolean device_node_timeout(gpointer data)
{
	double max_freq;
	int arrival;
	int deviceFd;

	/* Read the device node to update 'payloadData' object. */
	deviceFd = GPOINTER_TO_INT(data);
	read_device_node(deviceFd);

	/* Prepare the collected data for signal analysis. */
	prepare_samples();

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
gboolean model_keras_log_timeout(gpointer data)
{
	const char *kerasLog;

	/* Get the current text view and then update it. */
	modelTextBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(modelTextView));
	kerasLog = get_keras_script_log(MODEL_LOG_PATH);
	printLog("fetched the keras script log from related file");

	gtk_text_buffer_set_text(modelTextBuffer, kerasLog, -1);
	printLog("printed the keras script log within text view");

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
gboolean db_record_timeout(gpointer data)
{
	sqlite3 *db;

	db = (sqlite3 *) data;
	/* Bind the last sensor data. */
	db_bind_data(db, DATABASE_SENSOR_DATA);
	printLog("recorded the sensor data into '%s'", DB_SENSOR_DATA_PATH);

	return G_SOURCE_CONTINUE;
}

/**
 * Set the timeout for navigation updates.
 */
gboolean nav_update_timeout(gpointer data)
{
	/* Update the navigation data. */
	update_nav_data();
	printLog("updated the navigation data correctly");

	/* Select the direction and rotation for plot. */
	navAccel = accel_direction();
	navGyro = gyro_rotation();

	/* Request redraw for navigation plot. */
	gtk_widget_queue_draw(navPlotArea);
	printLog("requested the navigtion plot redraw");

	return G_SOURCE_CONTINUE;
}

/**
 * Set the timeout for GPS map updates.
 */
gboolean gps_update_timeout(gpointer)
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
 
