/**
 ******************************************************************************
 * @file 	navigation.c
 * @author 	Can Gulmez
 * @brief 	Navigation sensor headers of passive acoustic surveillance.
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

#define NAV_PLOT_MARGIN						0		/* pixel */
#define NAV_PLOT_GRID						20		/* pixel */
#define NAV_PLOT_CAXIS						220		/* pixel */
#define NAV_PLOT_VAXIS						280		/* pixel */
#define NAV_FLAT_GRAVITY					9.81	/* m/s^2 */
#define NAV_ACCEL_NOISE						1.0		/* m/s^2 */		
#define NAV_GYRO_NOISE						NAV_ACCEL_NOISE				
#define NAV_IMU_SENSOR						"LSM6DALTR (ST)"
#define NAV_SENSOR_ROW						8

/* Navigation enumerations */

typedef enum _NavAccel
{
	NAV_ACCEL_X_PLUS = 1,
	NAV_ACCEL_X_MINUS,
	NAV_ACCEL_Y_PLUS,
	NAV_ACCEL_Y_MINUS,
	NAV_ACCEL_Z_PLUS,
	NAV_ACCEL_Z_MINUS,
	NAV_ACCEL_UNDEF
} NavAccel;

typedef enum _NavGyro
{
	NAV_GYRO_X_PLUS = 1,
	NAV_GYRO_X_MINUS,
	NAV_GYRO_Y_PLUS,
	NAV_GYRO_Y_MINUS,
	NAV_GYRO_Z_PLUS,
	NAV_GYRO_Z_MINUS,
	NAV_GYRO_UNDEF
} NavGyro;

typedef enum _NavButton
{
	NAV_BUTTON_START = 1
} NavButton;

/* Nagivation shared widgets and variables */

extern NavAccel navAccel;
extern NavGyro navGyro;
extern GtkWidget *navPlotArea;
extern NavButton navButton;
extern GtkWidget *navSensorRows[NAV_SENSOR_ROW];
extern guint navTimeout;

/* Navigation function prototypes */

void navigation(GtkBox *imuBox, gpointer data);
GtkWidget *nav_info_group(gpointer data);
GtkWidget *nav_accel_group(gpointer data);
GtkWidget *nav_gyro_group(gpointer data);
GtkWidget *nav_magnet_group(gpointer data);
GtkWidget *nav_temp_group(gpointer data);
void nav_plot_area_grid(cairo_t *cr, int width, int height);
void nav_plot_area_device(cairo_t *cr, int width, int height);
void nav_plot_area_axes(cairo_t *cr, int width, int height);
void nav_plot_area_labels(cairo_t *cr, int width, int height);
void nav_plot_area_accel(cairo_t *cr, int width, int height, NavAccel accel);
void nav_plot_area_gyro(cairo_t *cr, int width, int height, NavGyro gyro);
void nav_plot_area(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data);

/* Navigation signal handler prototypes */

void on_nav_button_clicked(GtkButton *button, gpointer data);
