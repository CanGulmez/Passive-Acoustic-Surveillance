/**
 ******************************************************************************
 * @file 	main.c
 * @author 	Can GULMEZ
 * @brief 	Passive acoustic surveillance primarily designed for drones.
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

void on_activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window, *headerBar;
	GtkWidget *mainBox, *micBox, *modelBox, *navBox, *gpsBox;
	GtkWidget *viewSwitcher, *viewStack;
	GtkWidget *newBtn, *saveAsBtn, *trashBtn, *prefsBtn, *infoBtn, *avatarBtn;

	/* Main Window */

	window = adw_application_window_new(app);
	gtk_window_maximize(GTK_WINDOW(window));	

	/* Main Boxes */

	mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	micBox = __ui_page_box_new();
	modelBox = __ui_page_box_new();
	navBox = __ui_page_box_new();
	gpsBox = __ui_page_box_new();

	/* View Switchers */

	viewStack = adw_view_stack_new();
	visiblePageSig(viewStack, on_visible_page_changed);

	viewSwitcher = adw_view_switcher_new();
	adw_view_switcher_set_stack(ADW_VIEW_SWITCHER(viewSwitcher), ADW_VIEW_STACK(viewStack));
	adw_view_switcher_set_policy(ADW_VIEW_SWITCHER(viewSwitcher),
		ADW_VIEW_SWITCHER_POLICY_WIDE);

	/* Header Bar */

	headerBar = gtk_header_bar_new();
	gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(headerBar), TRUE);
	// gtk_widget_add_css_class(headerBar, "flat");
	gtk_header_bar_set_title_widget(GTK_HEADER_BAR(headerBar), viewSwitcher);
	gtk_box_append(GTK_BOX(mainBox), headerBar);

	newBtn = __ui_header_button_new("list-add-symbolic", "New");
	saveAsBtn = __ui_header_button_new("document-save-as-symbolic", "Save As");
	trashBtn = __ui_header_button_new("edit-delete-symbolic", "Trash");
	infoBtn = __ui_header_button_new("dialog-information-symbolic", "System");
	prefsBtn = __ui_header_button_new("preferences-system-symbolic", "Preferences");
	avatarBtn = __ui_header_button_new("avatar-default-symbolic", "About Me");

	gtk_header_bar_pack_start(GTK_HEADER_BAR(headerBar), newBtn);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(headerBar), saveAsBtn);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(headerBar), trashBtn);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(headerBar), avatarBtn);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(headerBar), prefsBtn);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(headerBar), infoBtn);

	/* View Stackes */

	adw_view_stack_add_titled(ADW_VIEW_STACK(viewStack), micBox, "microphone", "Microphone");
	adw_view_stack_page_set_icon_name(adw_view_stack_get_page(
		ADW_VIEW_STACK(viewStack), micBox), "audio-input-microphone-symbolic");
	
	adw_view_stack_add_titled(ADW_VIEW_STACK(viewStack), modelBox, "ai_model", "AI Model");
	adw_view_stack_page_set_icon_name(adw_view_stack_get_page(
		ADW_VIEW_STACK(viewStack), modelBox), "application-x-addon-symbolic");

	adw_view_stack_add_titled(ADW_VIEW_STACK(viewStack), navBox, "navigation", "Navigation");
	adw_view_stack_page_set_icon_name(adw_view_stack_get_page(
		ADW_VIEW_STACK(viewStack), navBox), "find-location-symbolic");
	
	adw_view_stack_add_titled(ADW_VIEW_STACK(viewStack), gpsBox, "gps_map", "GPS Map");
	adw_view_stack_page_set_icon_name(adw_view_stack_get_page(
		ADW_VIEW_STACK(viewStack), gpsBox), "applications-internet-symbolic");

	gtk_box_append(GTK_BOX(mainBox), viewStack);

	/* Modules */

	microphone(GTK_BOX(micBox), NULL);
	model(GTK_BOX(modelBox), NULL);
	navigation(GTK_BOX(navBox), NULL);
	gps_map(GTK_BOX(gpsBox), NULL);

	/* Presentation */
	
	adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), mainBox);
	gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[])
{
	int status;
	GtkApplication *app;
	
	adw_init();
	app = gtk_application_new("com.example.SONAR", G_APPLICATION_DEFAULT_FLAGS);
	activateSig(app, on_activate);

	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
 
