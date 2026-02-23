/**
 ******************************************************************************
 * @file 	navigation.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Navigation system of passive acoustic surveillance.
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

/* Shared widgets and variables */

NavAccel navAccel = NAV_ACCEL_UNDEF;
NavGyro navGyro = NAV_ACCEL_UNDEF;
GtkWidget *navPlotArea;
NavButton navButton;
GtkWidget *navSensorRows[8];	/* module, accel-status, accel-output,
											gyro-status, gyro-output, magnet-status,
											magnet-output, temp */
guint navTimeout = 0;

GtkWidget *nav_info_group(gpointer data)
{
	GtkWidget *infoGroup;

	infoGroup = __generic_group_new(
		"IMU Sensor", "Show the used IMU sensor information"
	);
	navSensorRows[0] = __generic_action_row_new("Sensor Series", "Null");
	__generic_group_add(infoGroup, navSensorRows[0]);

	return infoGroup;
}

GtkWidget *nav_accel_group(gpointer data)
{
	GtkWidget *accelGroup;

	accelGroup = __generic_group_new(
		"Accelerometer", "Show the accelerometer status and data (m/s^2)"
	);
	navSensorRows[1] = __generic_action_row_new("Status", "Not Running");
	__generic_group_add(accelGroup, navSensorRows[1]);
	navSensorRows[2] = __generic_action_row_new("Output", "[Null, Null, Null]");
	__generic_group_add(accelGroup, navSensorRows[2]);

	return accelGroup;
}

GtkWidget *nav_gyro_group(gpointer data)
{
	GtkWidget *gyroGroup;

	gyroGroup = __generic_group_new(
		"Gyroscope", "Show the gyroscope status and data (dps)"
	);
	navSensorRows[3] = __generic_action_row_new("Status", "Not Running");
	__generic_group_add(gyroGroup, navSensorRows[3]);
	navSensorRows[4] = __generic_action_row_new("Output", "[Null, Null, Null]");
	__generic_group_add(gyroGroup, navSensorRows[4]);

	return gyroGroup;
}

GtkWidget *nav_magnet_group(gpointer data)
{
	GtkWidget *magnetGroup;

	magnetGroup = __generic_group_new(
		"Magnetometer", "Show the magnetometer status and data"
	);
	navSensorRows[5] = __generic_action_row_new("Status", "Not Running");
	__generic_group_add(magnetGroup, navSensorRows[5]);
	navSensorRows[6] = __generic_action_row_new("Output", "[Null, Null, Null]");
	__generic_group_add(magnetGroup, navSensorRows[6]);

	return magnetGroup;
}

GtkWidget *nav_temp_group(gpointer data)
{
	GtkWidget *tempGroup;

	tempGroup = __generic_group_new(
		"Temperature", "Show the temperature data"
	);
	navSensorRows[7] = __generic_action_row_new("Output", "Null");
	__generic_group_add(tempGroup, navSensorRows[7]);

	return tempGroup;
}

void navigation(GtkBox *imuBox, gpointer data)
{
	GtkWidget *leftBox, *separator, *rightBox;
	GtkWidget *accelGroup, *gyroGroup, *magnetGroup, *tempGroup,
				 *infoGroup;	
	GtkWidget *propertyBox, *scrolledWin, *startBtn;

	/* Put the required boxes. */
	leftBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
	separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	rightBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	propertyBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	scrolledWin = gtk_scrolled_window_new();

	gtk_widget_set_hexpand(rightBox, TRUE);
	gtk_widget_set_vexpand(rightBox, TRUE);
	gtk_widget_set_vexpand(propertyBox, TRUE);

	gtk_widget_set_size_request(leftBox, 300, -1);

	gtk_widget_set_margin_end(leftBox, BOX_INNER_MARGIN);
	gtk_widget_set_margin_start(rightBox, BOX_INNER_MARGIN);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWin), 
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWin),
		propertyBox);

	/* Put the accelerometer, gyroscope and gyroscope groups. */
	infoGroup = nav_info_group(NULL);
	accelGroup = nav_accel_group(NULL);
	gyroGroup = nav_gyro_group(NULL);
	magnetGroup = nav_magnet_group(NULL);
	tempGroup = nav_temp_group(NULL);

	/* Put the required buttons. */
	startBtn = __generic_button_new("Start", "suggested-action");
	buttonSig(startBtn, on_nav_button_clicked);

	/* Put the plot areas. */
	navPlotArea = gtk_drawing_area_new();
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(navPlotArea), 
		nav_plot_area, NULL, NULL);

	gtk_widget_set_hexpand(navPlotArea, TRUE);
	gtk_widget_set_vexpand(navPlotArea, TRUE);
	gtk_widget_set_size_request(navPlotArea, 900, -1);

	/* Append the all groups and widgets to main box. */
	gtk_box_append(GTK_BOX(propertyBox), infoGroup);
	gtk_box_append(GTK_BOX(propertyBox), accelGroup);
	gtk_box_append(GTK_BOX(propertyBox), gyroGroup);
	gtk_box_append(GTK_BOX(propertyBox), magnetGroup);
	gtk_box_append(GTK_BOX(propertyBox), tempGroup);

	gtk_box_append(GTK_BOX(leftBox), scrolledWin);
	gtk_box_append(GTK_BOX(leftBox), startBtn);
	gtk_box_append(GTK_BOX(rightBox), navPlotArea);

	gtk_box_append(imuBox, leftBox);
	gtk_box_append(imuBox, separator);
	gtk_box_append(imuBox, rightBox);
}
