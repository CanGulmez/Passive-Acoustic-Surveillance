/**
 ******************************************************************************
 * @file 	gps-map.c
 * @author 	Can Gulmez
 * @brief 	GPS map headers of passive acoustic surveillance.
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

#include "__ui.h"

/* Global macro definitions */

#define GPS_ZOOM_LEVEL						12.0
#define GPS_INIT_LAT						41.008
#define GPS_INIT_LONG						28.9784
#define GPS_MODULE							"GY-NEO6MV2"
#define GPS_OUTPUT_ROWS						11

/* GPS map enumerations */

typedef enum _GpsButton
{
	GPS_BUTTON_START = 1
} GpsButton;

/* GPS map shared widgets and variables */

extern ShumateMarkerLayer *gpsMarkerLayer;
extern ShumateMap *gpsMap;
extern GpsButton gpsButton;
extern guint gpsTimeout;
extern GtkWidget *gpsModuleRows[GPS_OUTPUT_ROWS];

/* GPS map function prototypes */

void gps_map(GtkBox *mapBox);
void gps_map_area(GtkBox *rightBox);
void gps_map_area_markers(ShumateMarkerLayer *gpsMarkerLayer, double lat, double lon);

/* GPS map signal handler prototypes */

void on_gps_button_clicked(GtkButton *button, gpointer data);
