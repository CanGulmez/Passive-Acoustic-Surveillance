/**
 ******************************************************************************
 * @file 	signals.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Signal handlers of passive acoustic surveillance.
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

void on_visible_page_changed(GObject *object, GParamSpec *pspec, gpointer data)
{
	AdwViewStack *stack;
	const char *page;
	
	stack = ADW_VIEW_STACK(object);
	page = adw_view_stack_get_visible_child_name(stack);
	printLog("%s(): '%s'", FUNC, page);
	
	if (cmp(page, "microphone"))
	{
		currentPage = PAGE_MICROPHONE;
	}
	else if (cmp(page, "ai_model"))
	{
		currentPage = PAGE_AI_MODEL;
	}
	else if (cmp(page, "navigation"))
	{
		currentPage = PAGE_NAVIGATION;
	}
	else if (cmp(page, "gps_map"))
	{
		currentPage = PAGE_GPS_MAP;
	}
	else
	{
		customError("Unknown page: '%s'", page);
	}
}

void on_header_button_clicked(GtkButton *button, gpointer data)
{
	const char *icon;

	icon = gtk_button_get_icon_name(button);
	printLog("%s(): '%s'", FUNC, icon);

	if (cmp(icon, "list-add-symbolic"))
	{
		headerButton = HEADER_BUTTON_NEW;
	}
	else if (cmp(icon, "document-save-as-symbolic"))
	{
		headerButton = HEADER_BUTTON_SAVE_AS;
	}
	else if (cmp(icon, "edit-delete-symbolic"))
	{
		headerButton = HEADER_BUTTON_TRASH;
	}
	else if (cmp(icon, "dialog-information-symbolic"))
	{
		headerButton = HEADER_BUTTON_INFO;
	}
	else if (cmp(icon, "preferences-system-symbolic"))
	{
		headerButton = HEADER_BUTTON_PREFS;
	}
	else if (cmp(icon, "avatar-default-symbolic"))
	{
		headerButton = HEADER_BUTTON_AVATAR;
	}
	else
	{
		customError("Unknown header bar button: '%s'", icon);
	}
}

/*****************************************************************************/
/*****************************************************************************/

void on_comm_channel_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	int numDev;
	guint selected;
	GtkWidget *child;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected)  
	{
		case 0:	micChannel = MIC_CHANNEL_UART;	break;
		case 1:	micChannel = MIC_CHANNEL_USB;		break;
		case 2:	micChannel = MIC_CHANNEL_WIFI;	break;
		default:	
			customError("Unknown combo row selection");
	}
	numDev = get_device_nodes(micChannel);
	micDeviceNodes[numDev] = NULL;	/* NULL-terminating for GTK list */

	/* First, remove the current property group. */
	child = gtk_widget_get_first_child(GTK_WIDGET(data));
	if (child != NULL)
	{
		gtk_box_remove(GTK_BOX(data), child);
	}

	if (micChannel == MIC_CHANNEL_UART) 
	{
		mic_group_UART(NULL);
		gtk_box_append(GTK_BOX(data), micUARTGroup);
	} 
	else if (micChannel == MIC_CHANNEL_USB) 
	{
		mic_group_USB(NULL);	
		gtk_box_append(GTK_BOX(data), micUSBGroup);	
	} 
	else if (micChannel == MIC_CHANNEL_WIFI) 
	{
		mic_group_WiFi(NULL);	
		gtk_box_append(GTK_BOX(data), micWiFiGroup);	
	}
}

void on_device_node_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	AdwComboRow *comboRow;
	guint selected;
	char *item;
	GListModel *mod;

	comboRow = ADW_COMBO_ROW(gobject);
	selected = adw_combo_row_get_selected(comboRow);
	mod = adw_combo_row_get_model(comboRow);
	item = (char *) gtk_string_list_get_string(GTK_STRING_LIST(mod), selected);
	printLog("%s(): '%s'", FUNC, item);

	/* Clear the current device node and fill it with new */
	micDeviceNode = item;
}

void on_baud_rate_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	micBaudRate = MIC_BAUD_RATE_9600;		break;
		case 1:  micBaudRate = MIC_BAUD_RATE_14400;		break;
		case 2:	micBaudRate = MIC_BAUD_RATE_19200;		break;
		case 3:	micBaudRate = MIC_BAUD_RATE_28800;		break;
		case 4:	micBaudRate = MIC_BAUD_RATE_38400;		break;
		case 5:	micBaudRate = MIC_BAUD_RATE_57600;		break;
		case 6:	micBaudRate = MIC_BAUD_RATE_115200;		break;
		default:	
			customError("Unknown combo row selection");
	}
}

void on_data_bits_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	micDataBits = MIC_DATA_BITS_5;	break;
		case 1:	micDataBits = MIC_DATA_BITS_6;	break;
		case 2:	micDataBits = MIC_DATA_BITS_7;	break;
		case 3:	micDataBits = MIC_DATA_BITS_8;	break;
		default:	
			customError("Unknown combo row selection");
	}
}

void on_parity_bit_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	micParityBit = MIC_PARITY_BIT_NONE;		break;
		case 1:	micParityBit = MIC_PARITY_BIT_EVEN;		break;
		case 2:	micParityBit = MIC_PARITY_BIT_ODD;		break;
		case 3:	micParityBit = MIC_PARITY_BIT_MARK;		break;
		case 4:	micParityBit = MIC_PARITY_BIT_SPACE;	break;
		default:
			customError("Unknown combo row selection");
	}
}

void on_stop_bits_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	micStopBits = MIC_STOP_BITS_1;	break;
		case 1:	micStopBits = MIC_STOP_BITS_2;	break;
		default:
			customError("Unknown combo row selection");
	}
}

void on_flow_control_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	micFlowControl = MIC_FLOW_CONTROL_NONE;		break;
		case 1:	micFlowControl = MIC_FLOW_CONTROL_HARDWARE;	break;
		case 2:	micFlowControl = MIC_FLOW_CONTROL_SOFTWARE;	break;
		default:	
			customError("Unknown combo row selection");	
	}
}

/*****************************************************************************/
/*****************************************************************************/

void on_dataset_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	AdwComboRow *comboRow;
	guint selected;
	char *item;
	GListModel *mod;

	comboRow = ADW_COMBO_ROW(gobject);
	selected = adw_combo_row_get_selected(comboRow);
	mod = adw_combo_row_get_model(comboRow);
	item = (char *) gtk_string_list_get_string(GTK_STRING_LIST(mod), selected);
	printLog("%s(): '%s'", FUNC, item);

	/* Clear the current device node and fill it with new. */
	modelDataset = item;
}

void on_layer_type_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	modelLayerType = MODEL_LAYER_TYPE_LSTM;	break;
		case 1:	modelLayerType = MODEL_LAYER_TYPE_GRU;		break;
		default:
			customError("Unknown combo row selection");
	}
}

void on_recurrent_dropout_changed(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	gdouble value;
	AdwSpinRow *spinRow;

	spinRow = ADW_SPIN_ROW(gobject); 
	value = adw_spin_row_get_value(spinRow);
	printLog("%s(): '%.2f'", FUNC, value);

	modelDropout = value;
}

void on_units_changed(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	/* Call the generic spin row signal and get the selected item. */
	modelUnits = __generic_row_changed(gobject, pspec, data, FUNC);
}

void on_layer_number_changed(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	/* Call the generic spin row signal and get the selected item. */
	modelLayerNumber = __generic_row_changed(gobject, pspec, data, FUNC);
}

void on_batch_size_selected(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __generic_row_selected(gobject, pspec, data, FUNC);

	switch (selected) 
	{
		case 0:	modelBatchSize = MODEL_BATCH_SIZE_16;	break;
		case 1:	modelBatchSize = MODEL_BATCH_SIZE_32;	break;
		case 2:	modelBatchSize = MODEL_BATCH_SIZE_64;	break;
		case 3:	modelBatchSize = MODEL_BATCH_SIZE_128;	break;
		case 4:	modelBatchSize = MODEL_BATCH_SIZE_256;	break;
		case 5:	modelBatchSize = MODEL_BATCH_SIZE_512;	break;
		default:
			customError("Unknown combo row selection");
	}
}

void on_epochs_changed(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	/* Call the generic spin row signal and get the selected item. */
	modelEpochs = __generic_row_changed(gobject, pspec, data, FUNC);
}

void on_early_stop_switched(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	gboolean isActive;

	/* Call the generic switch row signal and get the active state. */
	isActive = __generic_row_switched(gobject, pspec, data, FUNC);

	switch (isActive) 
	{
		case 0:	modelEarlyStop = MODEL_EARLY_STOP_FALSE;	break;
		case 1:	modelEarlyStop = MODEL_EARLY_STOP_TRUE;	break;
		default:
			customError("Unknown combo row selection");
	}
}

void on_output_model_texted(GObject *gobject, GParamSpec *pspec, gpointer data)
{
	/* Call the generic entry row signal and then get the current text. */
	modelOutputName = (char *) __generic_row_texted(gobject, pspec, data, FUNC);
}

/*****************************************************************************/
/*****************************************************************************/

void on_mic_button_clicked(GtkButton *button, gpointer data)
{
	const char *label;
	static struct sqlite3 *db = NULL;
	static int deviceFd = -1;

	label = gtk_button_get_label(button);
	printLog("%s(): '%s'", FUNC, label);

	if (cmp(label, "Start"))
	{
		micButton = MIC_BUTTON_START;
	}
	else if (cmp(label, "Analyze"))
	{
		micButton = MIC_BUTTON_ANALYZE;
	}
	else if (cmp(label, "Stop"))
	{
		micButton = MIC_BUTTON_STOP;
	}
	else
	{
		customError("Unknown microphone button: '%s'", label);
	}

	/* When the buttons clicked, take the required actions. */
	if (micButton == MIC_BUTTON_START) 
	{
		/* Open the selected device node. */
		deviceFd = open_device_node(micChannel, micDeviceNode);

		/* Open the 'sensor_data.db' database. */
		db = db_open(DB_SENSOR_DATA_PATH);
		db_create_table(db, DATABASE_SENSOR_DATA);

		/* Add the timeout for updating "payloadData". */
		if (!micTimeout) 
		{
			micTimeout = g_timeout_add(TIMEOUT_DEVICE_READ, 
				timeout_device_node, GINT_TO_POINTER(deviceFd));
		}
		/* Add the timeout for recording sensor data into database. */
		if (!recordTimeout)
		{
			recordTimeout = g_timeout_add(TIMEOUT_DATA_RECORD,
				timeout_db_record, (gpointer) db);
		}
	} 
	else if (micButton == MIC_BUTTON_STOP) 
	{
		/* Close the open database. */
		if (db != NULL)
		{
			db_close(db);
		}
		/* Close also the open device node. */
		if (deviceFd != -1)
		{
			if (close(deviceFd) == -1)
			syscallError();
		}

		/* Stop the timeout for "payloadData". */
		if (micTimeout) 
		{
			g_source_remove(micTimeout);
			micTimeout = 0;
		}
		/* Stop the timeout for recording sensor data. */
		if (recordTimeout)
		{
			g_source_remove(recordTimeout);
			recordTimeout = 0;
		}
	} 
}

void on_model_button_clicked(GtkButton *button, gpointer data)
{
	const char *label;

	label = gtk_button_get_label(button);
	printLog("%s(): '%s'", FUNC, label);

	if (cmp(label, "Fit"))
	{
		modelButton = MODEL_BUTTON_FIT;
	}
	else if (cmp(label, "Abort"))
	{
		modelButton = MODEL_BUTTON_ABORT;
	}
	else if (cmp(label, "Evaluate"))
	{
		modelButton = MODEL_BUTTON_EVALUATE;
	}
	else if (cmp(label, "Predict"))
	{
		modelButton = MODEL_BUTTON_PREDICT;
	}
	else
	{
		customError("Unknown AI model button: '%s'", label);
	}

	/* Clear the text view area firstly and then update it. */
	modelTextBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(modelTextView));
	gtk_text_buffer_set_text(modelTextBuffer, " ", -1);
	
	/* According to clicked button, take required actions. */
	if (modelButton == MODEL_BUTTON_FIT) 
	{
		if (!modelFitPid) 
		{
			printLog("running the Keras script of '%s'", MODEL_FIT_SCRIPT);
			printLog("recording the Keras script log to '%s'", MODEL_LOG_PATH);
			modelFitPid = run_keras_script(MODEL_FIT_SCRIPT);	/* run the Keras model */

			if (!modelTimeout)
			{
				modelTimeout = g_timeout_add(TIMEOUT_MODEL_LOG, 
					timeout_model_keras_log, NULL);	/* start timeout for model */
			}
		}
	} 
	else if (modelButton == MODEL_BUTTON_ABORT) 
	{		
		if (modelFitPid) 
		{
			printLog("aborted the running Keras script (%d)", modelFitPid);
			abort_keras_script(modelFitPid);	/* abort the Keras script */
			modelFitPid = 0;
			
			if (modelTimeout)
			{
				g_source_remove(modelTimeout);	/* remove the model timeout */
				modelTimeout = 0;
			}
		}
	} 
	else if (modelButton == MODEL_BUTTON_EVALUATE) 
	{

	} 
	else if (modelButton == MODEL_BUTTON_PREDICT) 
	{

	}
}

void on_nav_button_clicked(GtkButton *button, gpointer data)
{
	const char *label;

	label = gtk_button_get_label(button);
	printLog("%s(): '%s'", FUNC, label);

	if (cmp(label, "Start"))
	{
		navButton = NAV_BUTTON_START;
	}

	/* If the 'Start' button is clicked, activate the readings. */
	if (navButton == NAV_BUTTON_START)
	{
		if (!navTimeout)
		{
			/* Start the timeout for real-time tracking. */
			g_timeout_add(TIMEOUT_NAV_UPDATE, timeout_nav_update, NULL);
		}
	}
}

void on_gps_button_clicked(GtkButton *button, gpointer data)
{
	const char *label;

	label = gtk_button_get_label(button);
	printLog("%s(): '%s'", FUNC, label);

	if (cmp(label, "Start"))
	{
		gpsButton = GPS_BUTTON_START;
	}

	/* If 'Start' button is clicked, activate the readings. */
	if (gpsButton == GPS_BUTTON_START)
	{
		if (!gpsTimeout)
		{
			/* Start the timeout for real-time tracking. */
			g_timeout_add(TIMEOUT_GPS_UPDATE, timeout_gps_update, NULL);
		}
	}
}
 