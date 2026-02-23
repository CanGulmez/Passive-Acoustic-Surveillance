/**
 ******************************************************************************
 * @file 	microphone.c
 * @author 	Ahmet Can GULMEZ
 * @brief 	Microphone data integration of passive acoustic surveillance.
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

/* Shared widgets and variables */

char *micDeviceNodes[MAX_DEVICE_NODE] = {0};
GtkWidget *micUARTGroup;
GtkWidget *micUSBGroup;
GtkWidget *micWiFiGroup;
GtkWidget *micSignalRows[MIC_SIGNAL_NUM] = {0};
GtkWidget *micCarPlot;
GtkWidget *micPolarPlot;
MicChannel micChannel = MIC_CHANNEL_UART;
char *micDeviceNode = NULL;
MicBaudRate micBaudRate = MIC_BAUD_RATE_115200;
MicDataBits micDataBits = MIC_DATA_BITS_8;
MicParityBit micParityBit = MIC_PARITY_BIT_NONE;
MicStopBits micStopBits = MIC_STOP_BITS_1;
MicFlowControl micFlowControl = MIC_FLOW_CONTROL_NONE;
guint micTimeout = 0;
guint recordTimeout = 0;
MicButton micButton;
PayloadData payloadData = {0};

/**
 * Initialize the device node row.
 */
void mic_row_device_node(GtkWidget *propertyGroup)
{
	GtkWidget *deviceNodeRow;

	deviceNodeRow = __generic_combo_row_new(
		"Device Node", (const char **)micDeviceNodes, 0
	);
	__generic_group_add(propertyGroup, deviceNodeRow);
	realizeSig(deviceNodeRow, on_device_node_selected);
	comboRowSig(deviceNodeRow, on_device_node_selected);
}

/**
 * Initialize the baud rate row.
 */
void mic_row_baud_rate(GtkWidget *propertyGroup)
{
	GtkWidget *baudRateRow;

	baudRateRow = __generic_combo_row_new(
		"Baud Rate", (const char *[]) {"9600", "14400", "19200", "28800", 
		"38400", "57600", "115200", NULL}, 6
	);
	__generic_group_add(propertyGroup, baudRateRow);
	comboRowSig(baudRateRow, on_baud_rate_selected);
}

/**
 * Initialize the data bits row.
 */
void mic_row_data_bits(GtkWidget *propertyGroup)
{
	GtkWidget *dataBitsRow;

	dataBitsRow = __generic_combo_row_new(
		"Data Bits", (const char *[]) {"5", "6", "7", "8", NULL}, 3
	);
	__generic_group_add(propertyGroup, dataBitsRow);
	comboRowSig(dataBitsRow, on_data_bits_selected);
}

/**
 * Initialize the parity bit row.
 */
void mic_row_parity_bit(GtkWidget *propertyGroup)
{
	GtkWidget *parityBitRow;

	parityBitRow = __generic_combo_row_new(
		"Parity Bit", (const char *[]) {"None", "Even", "Odd", 
		"Mark", "Space", NULL}, 0
	);
	__generic_group_add(propertyGroup, parityBitRow);
	comboRowSig(parityBitRow, on_parity_bit_selected);
}

/**
 * Initialize the stop bits row.
 */
void mic_row_stop_bits(GtkWidget *propertyGroup)
{
	GtkWidget *stopBitsRow;

	stopBitsRow = __generic_combo_row_new(
		"Stop Bits", (const char *[]) {"1", "2", NULL}, 0
	);
	__generic_group_add(propertyGroup, stopBitsRow);
	comboRowSig(stopBitsRow, on_stop_bits_selected);
}

/**
 * Initialize the flow control row.
 */
void mic_row_flow_control(GtkWidget *propertyGroup)
{
	GtkWidget *flowControlRow;

	flowControlRow = __generic_combo_row_new(
		"Flow Control", (const char *[]) {"None", "Hardware (RTS/CTS)", 
		"Software (XON/XOFF)", NULL}, 0
	);
	__generic_group_add(propertyGroup, flowControlRow);
	comboRowSig(flowControlRow, on_flow_control_selected);
}

/**
 * Initialize the UART property group.
 */
void mic_group_UART(gpointer data)
{
	/* Set the UART property group with default settings. */
	micUARTGroup = __generic_group_new("UART Properties",
		"Select the UART channel properties");

	mic_row_device_node(micUARTGroup);		/* Device Node */
	mic_row_baud_rate(micUARTGroup);			/* Baud Rate */
	mic_row_data_bits(micUARTGroup);			/* Data Bits */
	mic_row_parity_bit(micUARTGroup);		/* Parity Bit */
	mic_row_stop_bits(micUARTGroup);			/* Stop Bits */
	mic_row_flow_control(micUARTGroup);		/* Flow Control */
}

/**
 * Initialize the USB property group.
 */
void mic_group_USB(gpointer data)
{
	/* Set the USB property group with default settings. */
	micUSBGroup = __generic_group_new("USB Properties",
		"Select the USB channel properties");

	mic_row_device_node(micUSBGroup);		/* Device Node */
	mic_row_baud_rate(micUSBGroup);			/* Baud Rate */
	mic_row_data_bits(micUSBGroup);			/* Data Bits */
	mic_row_parity_bit(micUSBGroup);			/* Parity Bit */
	mic_row_stop_bits(micUSBGroup);			/* Stop Bits */
	mic_row_flow_control(micUSBGroup);		/* Flow Control */
}

/**
 * Initialize the Wi-Fi property group.
 */
void mic_group_WiFi(gpointer data)
{
	/* Set the Wi-Fi property group with default settings. */
	micWiFiGroup = __generic_group_new("Wi-Fi Properties",
		"Select the Wi-Fi channel properties");

	mic_row_device_node(micWiFiGroup);		/* Device Node */
}

/**
 * Update the signal analysis group.
 */
void mic_signal_analysis(GtkWidget *analysisGroup)
{
	int i;

	/* Create the predefined action rows with null labels. */
	micSignalRows[0] = __generic_action_row_new("Maximum", "Null");
	micSignalRows[1] = __generic_action_row_new("Minimum", "Null");
	micSignalRows[2] = __generic_action_row_new("Mean", "Null");
	micSignalRows[3] = __generic_action_row_new("Standard Deviation", "Null");
	micSignalRows[4] = __generic_action_row_new("Energy", "Null");
	micSignalRows[5] = __generic_action_row_new("RMS", "Null");
	micSignalRows[6] = __generic_action_row_new("Power (dB)", "Null");
	micSignalRows[7] = __generic_action_row_new("Crest Factor", "Null");
	micSignalRows[8] = __generic_action_row_new("Skewness", "Null");
	micSignalRows[9] = __generic_action_row_new("Kurtosis", "Null");
	micSignalRows[10] = __generic_action_row_new("Variance", "Null");
	micSignalRows[11] = __generic_action_row_new("Arrival of Angle", "Null");
	micSignalRows[12] = __generic_action_row_new("Distance (m)", "Null");
	micSignalRows[13] = __generic_action_row_new("Coordinate", "Null");
	micSignalRows[14] = __generic_action_row_new("Estimated Target", "Null");

	/* Put the analysis rows into the group. */
	for (i = 0; i < MIC_SIGNAL_NUM; i++)
	{
		__generic_group_add(analysisGroup, micSignalRows[i]);
	}
}

/**
 * Microphone Data Page
 */
void microphone(GtkBox *micBox, gpointer data)
{
	int i;
	GtkWidget *rightBox, *rightSep, *centerBox, *leftSep, *leftBox;
	GtkWidget *scrolledComm, *scrolledSig, *propertyBox, *btnBox;
	GtkWidget *commGroup, *analysisGroup;
	GtkWidget *commRow; 
	GtkWidget *startBtn, *stopBtn;

	leftBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
	leftSep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	centerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	rightSep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	rightBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	propertyBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	btnBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	scrolledComm = gtk_scrolled_window_new();
	scrolledSig = gtk_scrolled_window_new();

	gtk_widget_set_hexpand(rightBox, TRUE);
	gtk_widget_set_vexpand(rightBox, TRUE);
	gtk_widget_set_hexpand(propertyBox, TRUE);
	gtk_widget_set_vexpand(propertyBox, TRUE);

	gtk_widget_set_size_request(leftBox, 300, -1);
	gtk_widget_set_size_request(centerBox, 300, -1);

	gtk_widget_set_margin_end(leftBox, BOX_INNER_MARGIN);
	gtk_widget_set_margin_start(centerBox, BOX_INNER_MARGIN);
	gtk_widget_set_margin_end(centerBox, BOX_INNER_MARGIN);
	gtk_widget_set_margin_start(rightBox, BOX_INNER_MARGIN);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledComm), 
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledComm),
		propertyBox);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledSig), 
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledSig),
		centerBox);

	/**
	 * Put the communication group to select the channel. According 
	 * to selected communication channel, related property group will 
	 * be set as visible so that the settings can be selected.
	 */
	commGroup = __generic_group_new("Communication",
		"Set communication channel for data transmission");

	commRow = __generic_combo_row_new(
		"Channel", (const char *[]){"UART", "USB", "Wi-Fi", NULL}, 0
	);
	__generic_group_add(commGroup, commRow);
	comboRowSigWithData(commRow, on_comm_channel_selected, propertyBox);

	/* Put the initial UART property box. */ 
	get_device_nodes(micChannel);
	mic_group_UART(NULL);

	gtk_box_append(GTK_BOX(propertyBox), micUARTGroup);

	/* Put the signal analysis of the captured data at center. */
	analysisGroup = __generic_group_new("Signal Analysis",
		"Display the static signal analysis results");

	mic_signal_analysis(analysisGroup);
	
	/* Put the buttons to manage the upper and lower plots. */
	startBtn = __generic_button_new("Start", "suggested-action");
	stopBtn = __generic_button_new("Stop", "destructive-action");
	GtkWidget *buttons[2] = {startBtn, stopBtn};

	for (i = 0; i < 2; i++) 
	{
		buttonSig(buttons[i], on_mic_button_clicked);	
		gtk_box_append(GTK_BOX(btnBox), buttons[i]);
	}

	/* Put the required plots to see the microphone data in different forms. */
	micCarPlot = gtk_drawing_area_new();
	micPolarPlot = gtk_drawing_area_new();

	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(micCarPlot), mic_plot_car, 
		NULL, NULL);
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(micPolarPlot), mic_plot_polar, 
		NULL, NULL);
		
	gtk_widget_set_hexpand(micCarPlot, TRUE);
	gtk_widget_set_vexpand(micCarPlot, TRUE);
	gtk_widget_set_hexpand(micPolarPlot, TRUE);
	gtk_widget_set_vexpand(micPolarPlot, TRUE);

	gtk_widget_set_size_request(micCarPlot, 600, -1);
	gtk_widget_set_size_request(micPolarPlot, 600, -1);

	/* Append the all defined widgets in the microphone data page. */
	gtk_box_append(GTK_BOX(leftBox), commGroup);
	gtk_box_append(GTK_BOX(leftBox), scrolledComm);
	gtk_box_append(GTK_BOX(leftBox), btnBox);
	gtk_box_append(GTK_BOX(centerBox), analysisGroup);
	gtk_box_append(GTK_BOX(rightBox), micCarPlot);
	gtk_box_append(GTK_BOX(rightBox), micPolarPlot);

	gtk_box_append(micBox, leftBox);
	gtk_box_append(micBox, leftSep);
	gtk_box_append(micBox, scrolledSig);
	gtk_box_append(micBox, rightSep);
	gtk_box_append(micBox, rightBox);
}
 
