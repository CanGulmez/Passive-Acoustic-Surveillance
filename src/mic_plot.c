/**
 ******************************************************************************
 * @file 	mic_plot.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Microphone plot integrations of passive acoustic surveillance. 
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
 * Draw the cartesian plot frame.
 */
void mic_plot_car_frame(cairo_t *cr, int width, int height)
{
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(cr, 3.0);

	cairo_move_to(cr, MIC_PLOT_MARGIN, MIC_PLOT_MARGIN / 2);
	cairo_line_to(cr, MIC_PLOT_MARGIN, height - MIC_PLOT_MARGIN);
	cairo_move_to(cr, MIC_PLOT_MARGIN, height - MIC_PLOT_MARGIN);
	cairo_line_to(cr, width - MIC_PLOT_MARGIN / 2, height - MIC_PLOT_MARGIN);
	cairo_move_to(cr, width - MIC_PLOT_MARGIN / 2, height - MIC_PLOT_MARGIN);
	cairo_line_to(cr, width - MIC_PLOT_MARGIN / 2, MIC_PLOT_MARGIN / 2);
	cairo_move_to(cr, width - MIC_PLOT_MARGIN / 2, MIC_PLOT_MARGIN / 2);
	cairo_line_to(cr, MIC_PLOT_MARGIN, MIC_PLOT_MARGIN / 2);

	cairo_stroke(cr);
}

/**
 * Draw the cartesian plot grid.
 */
void mic_plot_car_grid(cairo_t *cr, int width, int height)
{
	int i;

	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);	/* gray grid */
	cairo_set_line_width(cr, 0.5);

	/* Put the vertical grid lines */
	for (i = MIC_PLOT_MARGIN + MIC_PLOT_GRID;
		  i < width - (MIC_PLOT_MARGIN / 2); i += MIC_PLOT_GRID) 
	{
		cairo_move_to(cr, i, MIC_PLOT_MARGIN / 2);
		cairo_line_to(cr, i, height - MIC_PLOT_MARGIN);
	}
	cairo_stroke(cr);

	/* Put the horizontal grid lines */
	for (i = (MIC_PLOT_MARGIN / 2) + MIC_PLOT_GRID;
		  i < height - MIC_PLOT_MARGIN; i += MIC_PLOT_GRID) 
	{
		cairo_move_to(cr, MIC_PLOT_MARGIN, i);
		cairo_line_to(cr, width - (MIC_PLOT_MARGIN / 2), i);
	}
	cairo_stroke(cr);
}

/**
 * Draw the cartesian plot x-axis label.
 */
void mic_plot_car_label_x(cairo_t *cr, int width, int height)
{
	char buffer[64];

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);	/* black indices */
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size(cr, 15.0);

	cairo_move_to(cr, 250, height - MIC_PLOT_MARGIN + 20);
	
	snprintf(buffer, 64, "Sensor Data (%d-points)", DATA_SIZE);
	cairo_show_text(cr, buffer);	
}

/**
 * Draw the cartesian plot y-axis label.
 */
void mic_plot_car_label_y(cairo_t *cr, int width, int height)
{
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);	/* black indices */
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size(cr, 15.0);

	cairo_save(cr);
	cairo_move_to(cr, (MIC_PLOT_MARGIN - 10), 250);
	cairo_rotate(cr, -M_PI / 2);
	cairo_show_text(cr, "Amplitude (Normalized)");	
	cairo_restore(cr);
}

/**
 * Draw the cartesian plot read data.
 */
void mic_plot_car_data(cairo_t *cr, int width, int height)
{
	int i;
	double step, middle;

	cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);	
	cairo_set_line_width(cr, 2.0);

	/* Get the middle point and step interval. */
	middle = (MIC_PLOT_MARGIN / 2) + 
				 ((height - (MIC_PLOT_MARGIN * 3 / 2)) / 2);
	step = (width - (MIC_PLOT_MARGIN * 3 / 2)) / (DATA_SIZE - 1);
	
	/* Draw the sensor data itself. */
	cairo_move_to(cr, MIC_PLOT_MARGIN + 2, middle);
	for (i = 1; i <= (width - MIC_PLOT_MARGIN * 3 / 2) - 2; i++) 
	{
		if (i >= DATA_SIZE)
		{
			cairo_line_to(cr, MIC_PLOT_MARGIN + (step * i) + 2, 
				middle);	
		}
		else
		{
			cairo_line_to(cr, MIC_PLOT_MARGIN + (step * i) + 2, 
				middle - sigBeamformed.data[i - 1]);
		}
	}
	cairo_stroke(cr);
}

/**
 * Draw the cartesian plot area.
 */
void mic_plot_car(GtkDrawingArea *area, cairo_t *cr, int width, int height, 
						gpointer data)
{
	/* Set the background of plot area. */
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);	/* white background */
	cairo_paint(cr);

	mic_plot_car_frame(cr, width, height);		/* frame */
	mic_plot_car_grid(cr, width, height);		/* grid */
	mic_plot_car_label_x(cr, width, height);	/* x-Axis Label */
	mic_plot_car_label_y(cr, width, height);	/* y-Axis Label */
	mic_plot_car_data(cr, width, height);		/* data itself */
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/**
 * Draw the polar plot area frame.
 */
void mic_plot_polar_frame(cairo_t *cr, int width, int height)
{
	int i, center_x, center_y, radius;

	center_x = width / 2;
	center_y = height / 2;
	radius = (height - 2 * MIC_PLOT_MARGIN) / 2;

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);		/* outer arch circular */
	cairo_set_line_width(cr, 3.0);	

	cairo_move_to(cr, center_x - radius, center_y);
	cairo_arc(cr, center_x, center_y, radius, -M_PI, M_PI);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);	/* inner arch circles */
	cairo_set_line_width(cr, 3.0);	

	for (i = 1; i < 5; i++) 
	{
		cairo_move_to(cr, center_x - radius + i * MIC_PLOT_GRID, center_y);
		cairo_arc(cr, center_x, center_y, radius - i * MIC_PLOT_GRID, -M_PI, M_PI);	
	}
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);	/* sector lines */
	cairo_set_line_width(cr, 3.0);

	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x + radius, center_y);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x - radius, center_y);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x, center_y - radius);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x, center_y + radius);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x + radius - MIC_PLOT_MARGIN, 
					  center_y - radius + MIC_PLOT_MARGIN);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x + radius - MIC_PLOT_MARGIN, 
					  center_y + radius - MIC_PLOT_MARGIN);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x - radius + MIC_PLOT_MARGIN, 
					  center_y - radius + MIC_PLOT_MARGIN);
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x - radius + MIC_PLOT_MARGIN, 
					  center_y + radius - MIC_PLOT_MARGIN);
	cairo_stroke(cr);
}

/**
 * Draw the polar plot area degrees and directions.
 */
void mic_plot_polar_label(cairo_t *cr, int width, int height)
{
	int center_x, center_y, radius;

	center_x = width / 2;
	center_y = height / 2;
	radius = (height - 2 * MIC_PLOT_MARGIN) / 2;

	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);	/* black indices */
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, 
		CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size(cr, 18.0);
	
	cairo_move_to(cr, center_x - 25, center_y - radius - 5);
	cairo_show_text(cr, "North");
	// cairo_move_to(cr, center_x + radius - MIC_PLOT_MARGIN + 4, 
	// 				  center_y - radius + MIC_PLOT_MARGIN - 4);
	// cairo_show_text(cr, "45");
	cairo_line_to(cr, center_x + radius + 4, center_y + 4);
	cairo_show_text(cr, "East");
	// cairo_move_to(cr, center_x + radius - MIC_PLOT_MARGIN + 2, 
	// 				  center_y + radius - MIC_PLOT_MARGIN + 16);
	// cairo_show_text(cr, "135");
	cairo_move_to(cr, center_x - 20, center_y + radius + 20);
	cairo_show_text(cr, "South");
	// cairo_move_to(cr, center_x - radius + MIC_PLOT_MARGIN - 30, 
	// 				  center_y + radius - MIC_PLOT_MARGIN + 20);
	// cairo_show_text(cr, "225");
	cairo_move_to(cr, center_x - radius - 50, center_y + 5);
	cairo_show_text(cr, "West");
	// cairo_move_to(cr, center_x - radius + MIC_PLOT_MARGIN - 30, 
	// 				  center_y - radius + MIC_PLOT_MARGIN - 10);
	// cairo_show_text(cr, "315");
}

/**
 * Fill the area between `start` and `end` angles of polar plot.
 */
void mic_plot_polar_fill(cairo_t *cr, int width, int height, double start, 
								 double end)
{
	int center_x, center_y;
	double radius, start_rad, end_rad;
	double start_compass, end_compass;

	cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.75);

	/* Get the related calculations. */
	center_x = width / 2;
	center_y = height / 2;
	radius = (height - 2 * MIC_PLOT_MARGIN) / 2;

	start_rad = start * (M_PI / 180.0);
	end_rad = end * (M_PI / 180.0);
	start_compass = atan2(-cos(start_rad), sin(start_rad));
   end_compass = atan2(-cos(end_rad), sin(end_rad));

	/* Select the sector area and then fill up. */
	cairo_move_to(cr, center_x, center_y);
	cairo_line_to(cr, center_x + radius * cos(start_compass),
							center_y + radius * sin(start_compass));
	cairo_arc(cr, center_x, center_y, radius, start_compass, end_compass);
	cairo_close_path(cr);
	cairo_fill(cr);
}

/**
 * Fill the sectors of polar plot.
 */
void mic_plot_polar_sector(cairo_t *cr, int width, int height, int mic)
{
	switch (mic)
	{
		case 1: mic_plot_polar_fill(cr, width, height, -15.0, 15.0); break;
		case 2: mic_plot_polar_fill(cr, width, height, 30.0, 60.0); break;
		case 3: mic_plot_polar_fill(cr, width, height, 75.0, 105.0); break;
		case 4: mic_plot_polar_fill(cr, width, height, 120.0, 150.0); break;
		case 5: mic_plot_polar_fill(cr, width, height, 165.0, 195.0); break;
		case 6: mic_plot_polar_fill(cr, width, height, 210.0, 240.0); break;
		case 7: mic_plot_polar_fill(cr, width, height, 255.0, 285.0); break;
		case 8: mic_plot_polar_fill(cr, width, height, 300.0, 330.0); break;
	}
}

/**
 * Draw the polar plot area.
 */
void mic_plot_polar(GtkDrawingArea *area, cairo_t *cr, int width, int height, 
						  gpointer data)
{
	/* Set the background of plot area */
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);	/* white background */
	cairo_paint(cr);
	
	mic_plot_polar_frame(cr, width, height);	/* draw the frame */
	mic_plot_polar_label(cr, width, height);	/* put the labels */
	mic_plot_polar_sector(cr, width, height, 
		sigVolumest);									/* fill the sector */
}
 