/**
 ******************************************************************************
 * @file 	__ui.h
 * @author 	Can Gulmez
 * @brief 	Generic UI component headers of passive acoustic surveillance.
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

#include <adwaita.h>
#include <cairo/cairo.h>

/* Global macro definitions */

#define __UI_BUTTON_WIDTH					100 	/* pixel */	
#define __UI_BUTTON_HEIGHT					40  	/* pixel */	
#define __UI_MARGIN_WIDTH					20  	/* pixel */	
#define __UI_MARGIN_HEIGHT					10  	/* pixel */	
#define __UI_INNER_MARGIN					10  	/* pixel */	
#define __UI_SPACING						25  	/* pixel */		

/* Shorthands for GTK & Adwaita objects */

#define __ui_sig_activate(app, callback) (g_signal_connect(app, "activate", G_CALLBACK(callback), NULL))
#define __ui_sig_visible_page(page, callback) (g_signal_connect(page, "notify::visible-child", G_CALLBACK(callback), NULL))
#define __ui_sig_combo_row(row, callback) (g_signal_connect(row, "notify::selected", G_CALLBACK(callback), NULL))
#define __ui_sig_spin_row(row, callback) (g_signal_connect(row, "notify::value", G_CALLBACK(callback), NULL))
#define __ui_sig_switch_row(row, callback) (g_signal_connect(row, "notify::active", G_CALLBACK(callback), NULL))
#define __ui_sig_entry_row(row, callback) (g_signal_connect(row, "notify::text", G_CALLBACK(callback), NULL))
#define __ui_sig_button(button, callback) (g_signal_connect(button, "clicked", G_CALLBACK(callback), NULL))
#define __ui_sig_combo_row_data(row, callback, data) (g_signal_connect(row, "notify::selected", G_CALLBACK(callback), data))
#define __ui_sig_spin_row_data(row, callback, data) (g_signal_connect(row, "notify::value", G_CALLBACK(callback), data))
#define __ui_sig_button_data(button, callback, data) (g_signal_connect(button, "clicked", G_CALLBACK(callback), data))
#define __ui_sig_realize(widget, callback) (g_signal_connect(widget, "realize", G_CALLBACK(callback), NULL))
#define __ui_sig_render(widget, callback) (g_signal_connect(widget, "render", G_CALLBACK(callback), NULL))
#define __ui_sig_pressed(widget, callback) (g_signal_connect(widget, "pressed", G_CALLBACK(callback), NULL))
#define __ui_sig_released(widget, callback) (g_signal_connect(widget, "released", G_CALLBACK(callback), NULL))
#define __ui_sig_motion(widget, callback) (g_signal_connect(widget, "motion", G_CALLBACK(callback), NULL))

/* Generic component function prototypes */

GtkWidget *__ui_page_box_new(void);
GtkWidget *__ui_header_button_new(const char *icon, const char *tooltip);

GtkWidget *__ui_action_row_new(const char *title, const char *label);
void __ui_action_row_update(GtkWidget *row, const char *label);
GtkWidget *__ui_combo_row_new(const char *title, const char **strings, guint index);
GtkWidget *__ui_spin_row_new(const char *title, double value, double lower, double upper, double increment, guint digits);
GtkWidget *__ui_switch_row_new(const char *title);
GtkWidget *__ui_entry_row_new(const char *title);
GtkWidget *__ui_button_new(const char *label, const char *action);

guint __ui_combo_row_selected(GObject *gobject, GParamSpec *pspec, gpointer data, const char *func);
guint __ui_spin_row_changed(GObject *gobject, GParamSpec *pspec, gpointer data, const char *func);
gboolean __ui_switch_row_switched(GObject *gobject, GParamSpec *pspec, gpointer data, const char *func);
const char *__ui_entry_row_texted(GObject *gobject, GParamSpec *pspec, gpointer data, const char *func);

GtkWidget *__ui_group_new(const char *title, const char *description);
void __ui_group_add(GtkWidget *group, GtkWidget *row);
void __ui_group_remove(GtkWidget *group, GtkWidget *row);
