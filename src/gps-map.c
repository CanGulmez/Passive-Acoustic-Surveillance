/**
 ******************************************************************************
 * @file 	gps_map.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	GPS map interation of passive acoustic surveillance.
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

/* GPS map shared widgets and variables */

ShumateMarkerLayer *gpsMarkerLayer = {0};
ShumateMap *gpsMap = {0};
GtkWidget *gpsModuleRows[11];	/* module, time, latitude, longitude, quality,
											num of sat, altitude, status, speed, course,
											date */
GPSButton gpsButton;
guint gpsTimeout = 0;

/**
 * Put the markers at specific locations in the map.
 */
void gps_map_area_markers(ShumateMarkerLayer *gpsMarkerLayer, double lat, 
								  double lon)
{
	GtkWidget *image;
	ShumateMarker *marker;
	
	image = gtk_image_new_from_icon_name("process-stop");
	marker = shumate_marker_new();
	
	gtk_image_set_pixel_size(GTK_IMAGE(image), 24);
	gtk_widget_set_valign(GTK_WIDGET(marker), GTK_ALIGN_CENTER);
	gtk_widget_set_halign(GTK_WIDGET(marker), GTK_ALIGN_CENTER);
	
	shumate_location_set_location(SHUMATE_LOCATION(marker), lat, lon);
	shumate_marker_set_child(marker, image);

	shumate_marker_layer_add_marker(gpsMarkerLayer, marker);
}

/**
 * Put the GPS map area.
 */
void gps_map_area(GtkBox *rightBox, gpointer data)
{
	ShumateSimpleMap *simpleMap;
	ShumateViewport *viewport;
	ShumateMapSourceRegistry *registry;
	ShumateMapSource *mapSource;
	
	simpleMap = shumate_simple_map_new();
	gpsMap = shumate_simple_map_get_map(simpleMap);
	viewport = shumate_simple_map_get_viewport(simpleMap);
	
	registry = shumate_map_source_registry_new_with_defaults();
	mapSource = shumate_map_source_registry_get_by_id(registry,
		SHUMATE_MAP_SOURCE_OSM_MAPNIK);
	
	shumate_map_center_on(gpsMap, GPS_INIT_LAT, GPS_INIT_LONG);
	shumate_simple_map_set_map_source(simpleMap, mapSource);
	
	shumate_viewport_set_reference_map_source(viewport, mapSource);
	shumate_viewport_set_zoom_level(viewport, GPS_ZOOM_LEVEL);

	/* Create marker and path layers. */
	gpsMarkerLayer = shumate_marker_layer_new(viewport);
	shumate_map_add_layer(gpsMap, SHUMATE_LAYER(gpsMarkerLayer));

	/* Set the map area expandable as possible. */
	gtk_widget_set_size_request(GTK_WIDGET(simpleMap), 
		900, -1);
	gtk_widget_set_hexpand(GTK_WIDGET(simpleMap), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(simpleMap), TRUE);

   gtk_box_append(GTK_BOX(rightBox), GTK_WIDGET(simpleMap));
}

/**
 * GPS Map Page
 */
void gps_map(GtkBox *mapBox, gpointer data)
{
	int i;
	GtkWidget *rightBox, *leftBox, *separator;	
	GtkWidget *GPSModuleGroup, *GPSResGroup;
	GtkWidget *scrolledWin, *propertyBox;
	GtkWidget *startBtn;

	leftBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
	propertyBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	rightBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	scrolledWin = gtk_scrolled_window_new();

	gtk_widget_set_hexpand(leftBox, TRUE);
	gtk_widget_set_vexpand(leftBox, TRUE);
	gtk_widget_set_vexpand(propertyBox, TRUE);
	gtk_widget_set_hexpand(rightBox, TRUE);
	gtk_widget_set_vexpand(rightBox, TRUE);

	gtk_widget_set_margin_end(leftBox, BOX_INNER_MARGIN);
	gtk_widget_set_margin_start(rightBox, BOX_INNER_MARGIN);

	gtk_widget_set_size_request(leftBox, 300, -1);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWin),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWin), 
		propertyBox);

	/*	Show the used GPS module infomration */
	GPSModuleGroup = __generic_group_new("GPS Module",
		"Show the used GPS module information");

	gpsModuleRows[0] = __generic_action_row_new("Module Series", "Null");
	__generic_group_add(GPSModuleGroup, gpsModuleRows[0]);

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
	GPSResGroup = __generic_group_new("GPS Data",
		"Show the GPS module data simultaneously");

	/* Put the GPS module outputs. */
	gpsModuleRows[1] = __generic_action_row_new("UTC Time", "Null");
	gpsModuleRows[2] = __generic_action_row_new("Latitude", "Null");
	gpsModuleRows[3] = __generic_action_row_new("Longitude", "Null");
	gpsModuleRows[4] = __generic_action_row_new("Fix Quality", "Null");
	gpsModuleRows[5] = __generic_action_row_new("Number of Sattelites", "Null");
	gpsModuleRows[6] = __generic_action_row_new("Altitude", "Null");
	gpsModuleRows[7] = __generic_action_row_new("Status", "Null");
	gpsModuleRows[8] = __generic_action_row_new("Speed over Ground (knots)", "Null");
	gpsModuleRows[9] = __generic_action_row_new("Course over Ground (degrees)", "Null");
	gpsModuleRows[10] = __generic_action_row_new("Date", "Null");

	/* Put the outputs into group. */
	for (i = 1; i < 11; i++)
	{
		__generic_group_add(GPSResGroup, gpsModuleRows[i]);
	}

	/* Put the required buttons. */
	startBtn = __generic_button_new("Start", "suggested-action");
	buttonSig(startBtn, on_gps_button_clicked);
	
	/* Layout */
	gtk_box_append(GTK_BOX(propertyBox), GPSModuleGroup);
	gtk_box_append(GTK_BOX(propertyBox), GPSResGroup);

	gtk_box_append(GTK_BOX(leftBox), scrolledWin);
	gtk_box_append(GTK_BOX(leftBox), startBtn);
	gps_map_area(GTK_BOX(rightBox), NULL);

	gtk_box_append(mapBox, leftBox);
	gtk_box_append(mapBox, separator);
	gtk_box_append(mapBox, rightBox);
}
 