/**
 ******************************************************************************
 * @file 	__ui.c
 * @author 	Can Gulmez
 * @brief 	Generic UI components of passive acoustic surveillance.
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

#include "./main.h"

/* General shared variables */

GtkWidget *__ui_page_box_new(void)
{
	GtkWidget *pageBox;

	pageBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_margin_start(pageBox, __UI_MARGIN_WIDTH);
	gtk_widget_set_margin_top(pageBox, __UI_MARGIN_HEIGHT);
	gtk_widget_set_margin_end(pageBox, __UI_MARGIN_WIDTH);
	gtk_widget_set_margin_bottom(pageBox, __UI_MARGIN_HEIGHT);

	return pageBox;
}

GtkWidget *__ui_header_button_new(const char *icon, const char *tooltip)
{
	GtkWidget *headerBtn;

	headerBtn = gtk_button_new_from_icon_name(icon);
	gtk_widget_set_tooltip_text(headerBtn, tooltip);	
	__ui_sig_button(headerBtn, on_header_button_clicked);

	return headerBtn;
}

/*****************************************************************************/
/*****************************************************************************/

GtkWidget *__ui_combo_row_new(const char *title, const char **strings, 
							  guint index)
{
	GtkWidget *comboRow;
	GtkStringList *stringList;

	comboRow = adw_combo_row_new();
	adw_preferences_row_set_title(ADW_PREFERENCES_ROW(comboRow), title);
	
	stringList = gtk_string_list_new(strings);
	adw_combo_row_set_model(ADW_COMBO_ROW(comboRow), G_LIST_MODEL(stringList));
	adw_combo_row_set_selected(ADW_COMBO_ROW(comboRow), index);

	return comboRow;
}

GtkWidget *__ui_spin_row_new(const char *title, double value, double lower,
							 double upper, double increment, guint digits)
{
	GtkWidget *spinRow;
	GtkAdjustment *adjustment;

	adjustment = gtk_adjustment_new(value, lower, upper, increment, 1, 0);
	spinRow = adw_spin_row_new(adjustment, 1, digits);
	adw_preferences_row_set_title(ADW_PREFERENCES_ROW(spinRow), title);
	adw_spin_row_set_numeric(ADW_SPIN_ROW(spinRow), TRUE);

	return spinRow;
}

GtkWidget *__ui_action_row_new(const char *title, const char *label)
{
	GtkWidget *actionRow, *labelWid;

	actionRow = adw_action_row_new();
	labelWid = gtk_label_new(label);

	/* Set the text as scrollabe so that long strings can be fit into 
		the fixed-height action row. */
	gtk_label_set_single_line_mode(GTK_LABEL(labelWid), TRUE);
	gtk_label_set_ellipsize(GTK_LABEL(labelWid), PANGO_ELLIPSIZE_END);
	gtk_widget_set_halign(labelWid, GTK_ALIGN_END);
	gtk_widget_set_hexpand(labelWid, TRUE);

	adw_preferences_row_set_title(ADW_PREFERENCES_ROW(actionRow), title);
	adw_action_row_add_suffix(ADW_ACTION_ROW(actionRow), labelWid);

	/* Store the label widget as data of the action row. */
   g_object_set_data(G_OBJECT(actionRow), "suffix-label", labelWid);
	
	return actionRow;
}

void __ui_action_row_update(GtkWidget *row, const char *label)
{
   GtkWidget *suffix;
	
	suffix = g_object_get_data(G_OBJECT(row), "suffix-label");
   if (suffix && GTK_IS_LABEL(suffix)) 
	{
      gtk_label_set_text(GTK_LABEL(suffix), label);
   }
}

GtkWidget *__ui_switch_row_new(const char *title)
{
	GtkWidget *switchRow;

	switchRow = adw_switch_row_new();
	adw_preferences_row_set_title(ADW_PREFERENCES_ROW(switchRow), title);

	return switchRow;
}

GtkWidget *__ui_entry_row_new(const char *title)
{
	GtkWidget *entryRow;

	entryRow = adw_entry_row_new();
	adw_preferences_row_set_title(ADW_PREFERENCES_ROW(entryRow), title);

	return entryRow;
}

GtkWidget *__ui_button_new(const char *label, const char *action)
{
	GtkWidget *button;
	
	button = gtk_button_new_with_label(label);
	gtk_widget_add_css_class(button, "circular");
	if (action != NULL)
	{
		gtk_widget_add_css_class(button, action);
	}
	gtk_widget_set_size_request(button, __UI_BUTTON_WIDTH, __UI_BUTTON_HEIGHT);

	return button;
}

/*****************************************************************************/
/*****************************************************************************/

/**
 * Generic combo row signal handler to get the selected item
 */
guint __ui_combo_row_selected(GObject *object, GParamSpec *pspec, gpointer data, 
							  const char *func)
{
	(void)pspec;
	(void)data;

	AdwComboRow *comboRow;
	guint selected;
	const char *item;
	GListModel *mod;

	comboRow = ADW_COMBO_ROW(object);
	selected = adw_combo_row_get_selected(comboRow);
	mod = adw_combo_row_get_model(comboRow);
	item = gtk_string_list_get_string(GTK_STRING_LIST(mod), selected);
	print_log("%s(): '%s'", func, item);

	return selected;
}

/**
 * Generic spin row signal handler to get the value.
 */
guint __ui_spin_row_changed(GObject *object, GParamSpec *pspec, gpointer data, 
							const char *func)
{
	(void)pspec;
	(void)data;

	guint value;
	AdwSpinRow *spinRow;
	
	spinRow = ADW_SPIN_ROW(object);
	value = adw_spin_row_get_value(spinRow);
	print_log("%s(): '%d'", func, value);

	return value;
}

/**
 * Generic switch row signal handler to get active state.
 */
gboolean __ui_switch_row_switched(GObject *object, GParamSpec *pspec, gpointer data, 
								  const char *func)
{
	(void)pspec;
	(void)data;

	gboolean isActive;
	AdwSwitchRow *switchRow;

	switchRow = ADW_SWITCH_ROW(object);
	isActive = adw_switch_row_get_active(switchRow);
	print_log("%s(): '%d'", func, isActive);

	return isActive;
}

/**
 * Generic entry row signal handler to get current text.
 */
const char *__ui_entry_row_texted(GObject *object, GParamSpec *pspec, 
								   gpointer data, const char *func)
{
	(void)pspec;
	(void)data;
	
	const char *currentText;

	currentText = gtk_editable_get_text(GTK_EDITABLE(object));
	print_log("%s(): '%s'", func, currentText);
	
	return currentText;
}

/*****************************************************************************/
/*****************************************************************************/

/**
 * Generic group creation
 */
GtkWidget *__ui_group_new(const char *title, const char *description)
{
	GtkWidget *group;

	group = adw_preferences_group_new();
	adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(group), title);
	adw_preferences_group_set_description(ADW_PREFERENCES_GROUP(group),
		description);
	
	return group;
}

/**
 * Generic combo group add row
 */
void __ui_group_add(GtkWidget *group, GtkWidget *row)
{
	adw_preferences_group_add(ADW_PREFERENCES_GROUP(group), row);
}

/**
 * Generic combo group remove row
 */
void __ui_group_remove(GtkWidget *group, GtkWidget *row)
{ 
	adw_preferences_group_remove(ADW_PREFERENCES_GROUP(group), row);
}

