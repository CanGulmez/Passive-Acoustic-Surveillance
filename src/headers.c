/**
 ******************************************************************************
 * @file 	headers.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Header button actions of passive acoustic surveillance.
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
 * System Window
 */
void header_system_window(void)
{
	AdwAboutWindow *systemWind;

	systemWind = ADW_ABOUT_WINDOW(adw_about_window_new());

	/* Set the system window properties. */
	adw_about_window_set_application_name(systemWind, HEADER_SYSTEM_NAME);
	adw_about_window_set_application_icon(systemWind, HEADER_SYSTEM_ICON);
	adw_about_window_set_version(systemWind, HEADER_SYSTEM_VERSION);
	adw_about_window_set_website(systemWind, HEADER_SYSTEM_WEBSITE);
	adw_about_window_set_developer_name(systemWind, HEADER_SYSTEM_DEVELOPER);
	adw_about_window_set_license_type(systemWind, HEADER_SYSTEM_LICENSE);
	adw_about_window_set_comments(systemWind, 
		"Passive acoustic surveillance system primarily designed for drones.");

	/* Set the size of the system window. */
	gtk_widget_set_size_request(GTK_WIDGET(systemWind), 450, 600);

	gtk_window_present(GTK_WINDOW(systemWind));
}

void header_preferences_window(void)
{
	
}

void header_about_me_window(void)
{
	
}