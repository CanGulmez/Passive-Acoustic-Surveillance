/**
 ******************************************************************************
 * @file 	nav_plot.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Navigation plotting of AeroSONAR.
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

void nav_plot_area_grid(cairo_t *cr, int width, int height)
{
	int i;

	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);	/* gray grid */
	cairo_set_line_width(cr, 0.2);
	
	/* Put the vertical grid lines */
	for (i = (NAV_PLOT_MARGIN + NAV_PLOT_GRID);
		  i < (width - NAV_PLOT_MARGIN); i += NAV_PLOT_GRID) 
	{
		cairo_move_to(cr, i, NAV_PLOT_MARGIN);
		cairo_line_to(cr, i, height - NAV_PLOT_MARGIN);
	}
	cairo_stroke(cr);

	/* Put the horizontal grid lines */
	for (i = (NAV_PLOT_MARGIN + NAV_PLOT_GRID);
		  i <= (height - NAV_PLOT_MARGIN); i += NAV_PLOT_GRID) 
	{
		cairo_move_to(cr, NAV_PLOT_MARGIN, i);
		cairo_line_to(cr, width - NAV_PLOT_MARGIN, i);
	}
	cairo_stroke(cr);
}

void nav_plot_area_device(cairo_t *cr, int width, int height)
{
	double center_x, center_y;

	center_x = (width - (2 * NAV_PLOT_MARGIN)) / 2;
	center_y = (height - (2 * NAV_PLOT_MARGIN)) / 2;

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);	/* black grid */
	cairo_set_line_width(cr, 2.0);

	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x + 180, center_y - 180);
	cairo_line_to(cr, center_x - 150, center_y);
	cairo_line_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x, center_y + 150);
	cairo_line_to(cr, center_x + 180, center_y - 180);
	cairo_fill(cr);
}

void nav_plot_area_axes(cairo_t *cr, int width, int height)
{
	double center_x, center_y;

	center_x = (width - (2 * NAV_PLOT_MARGIN)) / 2;
	center_y = (height - (2 * NAV_PLOT_MARGIN)) / 2;

	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);	/* black axes */
	cairo_set_line_width(cr, 2.0);

	/* Draw the vertical (Z to -Z) axis. */
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x, center_y - NAV_PLOT_VAXIS);
	cairo_line_to(cr, center_x, center_y + NAV_PLOT_VAXIS);

	/* Draw the cross (X to -X) axis. */
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
	cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);

	/* Draw the cross (Y to -Y) axis. */
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
	cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);
	cairo_stroke(cr);
}

void nav_plot_area_labels(cairo_t *cr, int width, int height)
{
	double center_x, center_y;

	center_x = (width - (2 * NAV_PLOT_MARGIN)) / 2;
	center_y = (height - (2 * NAV_PLOT_MARGIN)) / 2;

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);	/* black labels */
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size(cr, 18.0);

	/* Put the Z and -Z labels. */
	cairo_move_to(cr, center_x - 5, center_y - NAV_PLOT_VAXIS - 5);
	cairo_show_text(cr, "Z");
	cairo_move_to(cr, center_x - 5, center_y + NAV_PLOT_VAXIS + 20);
	cairo_show_text(cr, "-Z");
	cairo_move_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS - 5);

	/* Put the X and -X labels. */
	cairo_show_text(cr, "X");
	cairo_move_to(cr, center_x - NAV_PLOT_CAXIS - 20, center_y + NAV_PLOT_CAXIS + 20);
	cairo_show_text(cr, "-X");
	cairo_move_to(cr, center_x - NAV_PLOT_CAXIS - 10, center_y - NAV_PLOT_CAXIS - 5);

	/* Put the Y and -Y labels. */
	cairo_show_text(cr, "Y");
	cairo_move_to(cr, center_x + NAV_PLOT_CAXIS + 5, center_y + NAV_PLOT_CAXIS + 20);
	cairo_show_text(cr, "-Y");	
}

void nav_plot_area_accel(cairo_t *cr, int width, int height, NavAccel navAccel)
{
	double center_x, center_y;

	center_x = (width - (2 * NAV_PLOT_MARGIN)) / 2;
	center_y = (height - (2 * NAV_PLOT_MARGIN)) / 2;

	cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);	/* blue axes */
	cairo_set_line_width(cr, 5.0);

	cairo_move_to(cr, center_x, center_y);
	switch (navAccel)
	{
		case NAV_ACCEL_X_PLUS:
			/* Draw the arrow from center to +X axis. */
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
			cairo_move_to(cr, center_x + NAV_PLOT_CAXIS - 15, center_y - NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS + 15);
			break;

		case NAV_ACCEL_Y_PLUS:
			/* Draw the arrow from center to +Y axis. */
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS + 15);
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS + 15, center_y - NAV_PLOT_CAXIS);
			break;

		case NAV_ACCEL_Z_PLUS:
			/* Draw the arrow from center to +Z axis. */
			cairo_line_to(cr, center_x, center_y - NAV_PLOT_VAXIS);
			cairo_move_to(cr, center_x - 10, center_y - NAV_PLOT_VAXIS + 10);
			cairo_line_to(cr, center_x, center_y - NAV_PLOT_VAXIS);
			cairo_line_to(cr, center_x + 10, center_y - NAV_PLOT_VAXIS + 10);
			break;

		case NAV_ACCEL_X_MINUS:
			/* Draw the arrow from center to -X axis. */
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);	
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS - 15);
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS + 15, center_y + NAV_PLOT_CAXIS);
			break;

		case NAV_ACCEL_Y_MINUS:
			/* Draw the arrow from center to -X axis. */
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);	
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS - 15);
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x - NAV_PLOT_CAXIS + 15, center_y + NAV_PLOT_CAXIS);
			break;

		case NAV_ACCEL_Z_MINUS:
			/* Draw the arrow from center to -Z axis. */
			cairo_line_to(cr, center_x, center_y + NAV_PLOT_VAXIS);
			cairo_move_to(cr, center_x - 10, center_y + NAV_PLOT_VAXIS - 10);
			cairo_line_to(cr, center_x, center_y + NAV_PLOT_VAXIS);
			cairo_line_to(cr, center_x + 10, center_y + NAV_PLOT_VAXIS - 10);
			break;

		case NAV_ACCEL_UNDEF:
			/* Not found any data so that leave it back. */
			break;
	}
	cairo_stroke(cr);
}

void nav_plot_area_gyro(cairo_t *cr, int width, int height, NavGyro navGyro)
{
	double center_x, center_y;

	center_x = (width - (2 * NAV_PLOT_MARGIN)) / 2;
	center_y = (height - (2 * NAV_PLOT_MARGIN)) / 2;

	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);	/* red axes */
	cairo_set_line_width(cr, 5.0);

	switch (navGyro)
	{
		case NAV_GYRO_X_PLUS:
			/* Draw the rotation from -X to +X axis. */
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);

			/* Draw the elliptical circle toward to left side. */
			break;

		case NAV_GYRO_X_MINUS:
			/* Draw the rotation from -X to +X axis. */
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);

			/* Draw the elliptical circle toward to right side. */
			break;

		case NAV_GYRO_Y_PLUS:
			/* Draw the rotation from -Y to +Y axis. */
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);

			/* Draw the elliptical circle toward to up side. */
			break;

		case NAV_GYRO_Y_MINUS:
			/* Draw the rotation from -Y to +Y axis. */
			cairo_move_to(cr, center_x - NAV_PLOT_CAXIS, center_y - NAV_PLOT_CAXIS);
			cairo_line_to(cr, center_x + NAV_PLOT_CAXIS, center_y + NAV_PLOT_CAXIS);

			/* Draw the elliptical circle toward to down side. */
			break;

		case NAV_GYRO_Z_PLUS:
			/* Draw the rotation from -Z to +Z axis. */
			cairo_move_to(cr, center_x, center_y - NAV_PLOT_VAXIS);
			cairo_line_to(cr, center_x, center_y + NAV_PLOT_VAXIS);

			/* Draw the elliptical circle toward to left side. */
			cairo_move_to(cr, center_x + 30, center_y - NAV_PLOT_VAXIS + 20);
			cairo_arc(cr, center_x, center_y - NAV_PLOT_VAXIS + 20, 30, 0, -M_PI);

			cairo_move_to(cr, center_x + 30 - 10, center_y - NAV_PLOT_VAXIS + 20 + 10);
			cairo_line_to(cr, center_x + 30, center_y - NAV_PLOT_VAXIS + 20);
			cairo_line_to(cr, center_x + 30 + 10, center_y - NAV_PLOT_VAXIS + 20 + 10);
			break;

		case NAV_GYRO_Z_MINUS:
			/* Draw the rotation from -Z to +Z axis. */
			cairo_move_to(cr, center_x, center_y - NAV_PLOT_VAXIS);
			cairo_line_to(cr, center_x, center_y + NAV_PLOT_VAXIS);

			/* Draw the elliptical circle toward to right side. */
			cairo_move_to(cr, center_x + 30, center_y - NAV_PLOT_VAXIS + 20);
			cairo_arc(cr, center_x, center_y - NAV_PLOT_VAXIS + 20, 30, 0, -M_PI);

			cairo_move_to(cr, center_x - 30 - 10, center_y - NAV_PLOT_VAXIS + 20 + 10);
			cairo_line_to(cr, center_x - 30, center_y - NAV_PLOT_VAXIS + 20);
			cairo_line_to(cr, center_x - 30 + 10, center_y - NAV_PLOT_VAXIS + 20 + 10);
			break;

		case NAV_GYRO_UNDEF:
			/* Not found any data so that leave it back. */
			break;
	}
	cairo_stroke(cr);
}

void nav_plot_area(GtkDrawingArea *area, cairo_t *cr, int width,
						 int height, gpointer data)
{
	/* Set the background of bottom plot area. */
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_paint(cr);

	nav_plot_area_grid(cr, width, height);		/* draw grids */
	nav_plot_area_device(cr, width, height);	/* draw device */
	nav_plot_area_axes(cr, width, height);		/* draw axes */
	nav_plot_area_labels(cr, width, height);	/* draw labels */

	nav_plot_area_accel(cr, width, height, navAccel);
	nav_plot_area_gyro(cr, width, height, navGyro);
}
 