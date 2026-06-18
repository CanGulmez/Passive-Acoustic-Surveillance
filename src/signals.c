/**
 ******************************************************************************
 * @file 	signals.c
 * @author 	Can Gulmez
 * @brief 	Signal handlers of passive acoustic surveillance.
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

#include "main.h"

/* General shared widgets and variables */

HeaderButton headerButton;
CurrentPage currentPage = PAGE_MICROPHONE;
PayloadData payloadData = {0};

void on_visible_page_changed(GObject *object, GParamSpec *pspec, gpointer data)
{
	(void)pspec;
	(void)data;

	AdwViewStack *stack;
	const char *page;
	
	stack = ADW_VIEW_STACK(object);
	page = adw_view_stack_get_visible_child_name(stack);
	print_log("%s(): '%s'", __func__, page);
	
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
		custom_error("Unknown page: '%s'", page);
	}
}

void on_header_button_clicked(GtkButton *button, gpointer data)
{
	(void)data;

	const char *icon;

	icon = gtk_button_get_icon_name(button);
	print_log("%s(): '%s'", __func__, icon);

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
		headerButton = HEADER_BUTTON_SYSTEM;
	}
	else if (cmp(icon, "preferences-system-symbolic"))
	{
		headerButton = HEADER_BUTTON_PREFS;
	}
	else if (cmp(icon, "avatar-default-symbolic"))
	{
		headerButton = HEADER_BUTTON_ABOUT;
	}
	else
	{
		custom_error("Unknown header bar button: '%s'", icon);
	}

	/* Take the required actions for clicked header buttons. */
	if (headerButton == HEADER_BUTTON_SYSTEM)
	{
		header_system_window();
	}
}

/*****************************************************************************/
/*****************************************************************************/

void on_comm_channel_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	int numDev;
	guint selected;
	GtkWidget *child;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected)  
	{
		case 0:	micChannel = MIC_CHANNEL_UART;	break;
		case 1:	micChannel = MIC_CHANNEL_USB;	break;
		case 2:	micChannel = MIC_CHANNEL_WIFI;	break;
		default:	
			custom_error("Unknown combo row selection");
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
		mic_group_UART();
		gtk_box_append(GTK_BOX(data), micUARTGroup);
	} 
	else if (micChannel == MIC_CHANNEL_USB) 
	{
		mic_group_USB();	
		gtk_box_append(GTK_BOX(data), micUSBGroup);	
	} 
	else if (micChannel == MIC_CHANNEL_WIFI) 
	{
		mic_group_WiFi();	
		gtk_box_append(GTK_BOX(data), micWiFiGroup);	
	}
}

void on_device_node_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	(void)pspec;
	(void)data;

	AdwComboRow *comboRow;
	guint selected;
	char *item;
	GListModel *mod;

	comboRow = ADW_COMBO_ROW(object);
	selected = adw_combo_row_get_selected(comboRow);
	mod = adw_combo_row_get_model(comboRow);
	item = (char *) gtk_string_list_get_string(GTK_STRING_LIST(mod), selected);
	print_log("%s(): '%s'", __func__, item);

	/* Clear the current device node and fill it with new */
	micDeviceNode = item;
}

void on_baud_rate_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	micBaudRate = MIC_BAUD_RATE_9600;		break;
		case 1:	micBaudRate = MIC_BAUD_RATE_19200;		break;
		case 2:	micBaudRate = MIC_BAUD_RATE_38400;		break;
		case 3:	micBaudRate = MIC_BAUD_RATE_57600;		break;
		case 4:	micBaudRate = MIC_BAUD_RATE_115200;		break;
		default:	
			custom_error("Unknown combo row selection");
	}
}

void on_data_bits_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	micDataBits = MIC_DATA_BITS_5;	break;
		case 1:	micDataBits = MIC_DATA_BITS_6;	break;
		case 2:	micDataBits = MIC_DATA_BITS_7;	break;
		case 3:	micDataBits = MIC_DATA_BITS_8;	break;
		default:	
			custom_error("Unknown combo row selection");
	}
}

void on_parity_bit_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	micParityBit = MIC_PARITY_BIT_NONE;		break;
		case 1:	micParityBit = MIC_PARITY_BIT_EVEN;		break;
		case 2:	micParityBit = MIC_PARITY_BIT_ODD;		break;
		default:
			custom_error("Unknown combo row selection");
	}
}

void on_stop_bits_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	micStopBits = MIC_STOP_BITS_1;	break;
		case 1:	micStopBits = MIC_STOP_BITS_2;	break;
		default:
			custom_error("Unknown combo row selection");
	}
}

void on_flow_control_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	micFlowCntl = MIC_FLOW_CNTL_NO;	break;
		case 1:	micFlowCntl = MIC_FLOW_CNTL_HW;	break;
		case 2:	micFlowCntl = MIC_FLOW_CNTL_SW;	break;
		default:	
			custom_error("Unknown combo row selection");	
	}
}

/*****************************************************************************/
/*****************************************************************************/

void on_dataset_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	(void)pspec;
	(void)data;

	AdwComboRow *comboRow;
	guint selected;
	char *item;
	GListModel *mod;

	comboRow = ADW_COMBO_ROW(object);
	selected = adw_combo_row_get_selected(comboRow);
	mod = adw_combo_row_get_model(comboRow);
	item = (char *) gtk_string_list_get_string(GTK_STRING_LIST(mod), selected);
	print_log("%s(): '%s'", __func__, item);

	/* Clear the current device node and fill it with new. */
	modelDataset = item;
}

void on_layer_type_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	modelLayerType = MODEL_LAYER_TYPE_LSTM;	break;
		case 1:	modelLayerType = MODEL_LAYER_TYPE_GRU;		break;
		default:
			custom_error("Unknown combo row selection");
	}
}

void on_recurrent_dropout_changed(GObject *object, GParamSpec *pspec, gpointer data)
{
	(void)pspec;
	(void)data;

	gdouble value;
	AdwSpinRow *spinRow;

	spinRow = ADW_SPIN_ROW(object); 
	value = adw_spin_row_get_value(spinRow);
	print_log("%s(): '%.2f'", __func__, value);

	modelDropout = value;
}

void on_units_changed(GObject *object, GParamSpec *pspec, gpointer data)
{
	/* Call the generic spin row signal and get the selected item. */
	modelUnits = __ui_spin_row_changed(object, pspec, data, __func__);
}

void on_layer_number_changed(GObject *object, GParamSpec *pspec, gpointer data)
{
	/* Call the generic spin row signal and get the selected item. */
	modelLayerNumber = __ui_spin_row_changed(object, pspec, data, __func__);
}

void on_batch_size_selected(GObject *object, GParamSpec *pspec, gpointer data)
{
	guint selected;

	/* Call the generic combo row signal and get the selected item. */
	selected = __ui_combo_row_selected(object, pspec, data, __func__);

	switch (selected) 
	{
		case 0:	modelBatchSize = MODEL_BATCH_SIZE_16;	break;
		case 1:	modelBatchSize = MODEL_BATCH_SIZE_32;	break;
		case 2:	modelBatchSize = MODEL_BATCH_SIZE_64;	break;
		case 3:	modelBatchSize = MODEL_BATCH_SIZE_128;	break;
		case 4:	modelBatchSize = MODEL_BATCH_SIZE_256;	break;
		case 5:	modelBatchSize = MODEL_BATCH_SIZE_512;	break;
		default:
			custom_error("Unknown combo row selection");
	}
}

void on_epochs_changed(GObject *object, GParamSpec *pspec, gpointer data)
{
	/* Call the generic spin row signal and get the selected item. */
	modelEpochs = __ui_spin_row_changed(object, pspec, data, __func__);
}

void on_early_stop_switched(GObject *object, GParamSpec *pspec, gpointer data)
{
	gboolean isActive;

	/* Call the generic switch row signal and get the active state. */
	isActive = __ui_switch_row_switched(object, pspec, data, __func__);

	switch (isActive) 
	{
		case 0:	modelEarlyStop = MODEL_EARLY_STOP_FALSE;	break;
		case 1:	modelEarlyStop = MODEL_EARLY_STOP_TRUE;	break;
		default:
			custom_error("Unknown combo row selection");
	}
}

void on_output_model_texted(GObject *object, GParamSpec *pspec, gpointer data)
{
	/* Call the generic entry row signal and then get the current text. */
	modelOutputName = (char *) __ui_entry_row_texted(object, pspec, data, __func__);
}

/*****************************************************************************/
/*****************************************************************************/

void on_mic_button_clicked(GtkButton *button, gpointer data)
{
	(void)data;

	const char *label;
	static sqlite3 *db = NULL;
	static int deviceFd = -1, s;

	label = gtk_button_get_label(button);
	print_log("%s(): '%s'", __func__, label);

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
		custom_error("Unknown microphone button: '%s'", label);
	}

	/* When the buttons clicked, take the required actions. */
	if (micButton == MIC_BUTTON_START) 
	{
		/* Open the selected device node. */
		deviceFd = open_device_node(micChannel, micDeviceNode);

		/* Open the 'sensor_data.db' database. */
		db = db_open(DB_SENSOR_PATH);
		db_create_table(db);

		/* Create the new thread for reading the serial line. */
		if (!micThreadStarted) 
		{
			micThreadStarted = true;
			s = pthread_create(&micThread, NULL, payload_data_thread, 
							   (void *)&deviceFd);
			if (s != 0)
				syscall_error();
		}
	} 
	else if (micButton == MIC_BUTTON_STOP) 
	{
		/* Cancel the thread. */
		if (micThreadStarted)
		{
			pthread_cancel(micThread);
			micThreadStarted = false;
		}	
		/* Close the open database. */
		if (db != NULL)
		{
			db_close(db);
		}
		/* Close also the open device node. */
		if (deviceFd != -1)
		{
			if (close(deviceFd) == -1)
			syscall_error();
		}
	} 
}

void on_model_button_clicked(GtkButton *button, gpointer data)
{
	(void)data;

	const char *label;

	label = gtk_button_get_label(button);
	print_log("%s(): '%s'", __func__, label);

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
		custom_error("Unknown AI model button: '%s'", label);
	}

	/* Clear the text view area firstly and then update it. */
	modelTextBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(modelTextView));
	gtk_text_buffer_set_text(modelTextBuffer, " ", -1);
	
	/* According to clicked button, take required actions. */
	if (modelButton == MODEL_BUTTON_FIT) 
	{
		if (!modelFitPid) 
		{
			print_log("running the Keras script of '%s'", MODEL_FIT_SCRIPT);
			print_log("recording the Keras script log to '%s'", MODEL_LOG_PATH);
			modelFitPid = run_keras_script(MODEL_FIT_SCRIPT);	/* run the Keras model */

			if (!modelTimeout)
			{
				modelTimeout = g_timeout_add(TIMEOUT_MODEL_LOG, 
					model_keras_log_timeout, NULL);	/* start timeout for model */
			}
		}
	} 
	else if (modelButton == MODEL_BUTTON_ABORT) 
	{		
		if (modelFitPid) 
		{
			print_log("aborted the running Keras script (%d)", modelFitPid);
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
