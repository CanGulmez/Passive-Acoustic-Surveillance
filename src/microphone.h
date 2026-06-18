/**
 ******************************************************************************
 * @file 	microphone.c
 * @author 	Can Gulmez
 * @brief 	Microphone integration headers of passive acoustic surveillance.
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

/* GLobal macro definations */

#define MIC_SERIAL_PATH						"/dev/"
#define MIC_WIFI_PATH						"/sys/class/net/"
#define MIC_UART_PREFIX						"ttyS"
#define MIC_USB_PREFIX						"ttyUSB"
#define MIC_WIFI_PREFIX						"wl"
#define MIC_PLOT_MARGIN						40		/* pixel */
#define MIC_PLOT_GRID						20		/* pixel */
#define MIC_SIGNAL_NUM						15
#define MIC_COUNT							4
#define MIC_RADIUS							0.1		/* meter */
#define MIC_SAMPLING_FREQ					48000	/* Hz */
#define MIC_DEVICE_NODE						64
#define MIC_MAGIC_WORD						-99

/* Microphone enumerations */

typedef enum _MicChannel
{
	MIC_CHANNEL_UART = 1,
	MIC_CHANNEL_USB,
	MIC_CHANNEL_WIFI
} MicChannel;

typedef enum _MicBaudRate
{
	MIC_BAUD_RATE_9600 = 1,
	MIC_BAUD_RATE_19200,
	MIC_BAUD_RATE_38400,
	MIC_BAUD_RATE_57600,
	MIC_BAUD_RATE_115200
} MicBaudRate;

typedef enum _MicDataBits
{
	MIC_DATA_BITS_5 = 1,
	MIC_DATA_BITS_6,
	MIC_DATA_BITS_7,
	MIC_DATA_BITS_8
} MicDataBits;

typedef enum _MicParityBit
{
	MIC_PARITY_BIT_NONE = 1,
	MIC_PARITY_BIT_EVEN,
	MIC_PARITY_BIT_ODD
} MicParityBit;

typedef enum _MicStopBits
{
	MIC_STOP_BITS_1 = 1,
	MIC_STOP_BITS_2
} MicStopBits;

typedef enum _MicFlowControl
{
	MIC_FLOW_CNTL_NO = 1,
	MIC_FLOW_CNTL_HW,
	MIC_FLOW_CNTL_SW
} MicFlowCntl;

typedef enum _MicButton
{
	MIC_BUTTON_START = 1,
	MIC_BUTTON_ANALYZE,
	MIC_BUTTON_STOP
} MicButton;

/* Microphone shared widgets and variables */

extern char *micDeviceNodes[MIC_DEVICE_NODE];
extern GtkWidget *micUARTGroup;
extern GtkWidget *micUSBGroup;
extern GtkWidget *micWiFiGroup;
extern GtkWidget *micSignalRows[MIC_SIGNAL_NUM];
extern GtkWidget *micCarPlot;
extern GtkWidget *micPolarPlot;
extern MicChannel micChannel;
extern char *micDeviceNode;
extern MicBaudRate micBaudRate;
extern MicDataBits micDataBits;
extern MicParityBit micParityBit;
extern MicStopBits micStopBits;
extern MicFlowCntl micFlowCntl;
extern pthread_t micThread;
extern pthread_mutex_t micMutex;
extern bool micThreadStarted;
extern MicButton micButton;

/* Microphone function prototypes */

void microphone(GtkBox *micBox);
void mic_row_device_node(GtkWidget *propertyGroup);						
void mic_row_baud_rate(GtkWidget *propertyGroup);	
void mic_row_data_bits(GtkWidget *propertyGroup);	
void mic_row_parity_bit(GtkWidget *propertyGroup);
void mic_row_stop_bits(GtkWidget *propertyGroup);	
void mic_row_flow_control(GtkWidget *propertyGroup);
void mic_group_UART(void);
void mic_group_USB(void);
void mic_group_WiFi(void);
void mic_signal_analysis(GtkWidget *analysisGroup);
void mic_plot_car(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data);
void mic_plot_car_frame(cairo_t *cr, int width, int height);	
void mic_plot_car_grid(cairo_t *cr, int width, int height);	
void mic_plot_car_label_x(cairo_t *cr, int height);		
void mic_plot_car_label_y(cairo_t *cr);	
void mic_plot_car_data(cairo_t *cr, int width, int height);
void mic_plot_polar(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data);
void mic_plot_polar_frame(cairo_t *cr, int width, int height);
void mic_plot_polar_label(cairo_t *cr, int width, int height);
void mic_plot_polar_fill(cairo_t *cr, int width, int height, double start, double end);
void mic_plot_polar_sector(cairo_t *cr, int width, int height, int mic);

/* Microphone signal handler prototypes */

void on_comm_channel_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_device_node_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_baud_rate_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_data_bits_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_parity_bit_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_stop_bits_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_flow_control_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_mic_button_clicked(GtkButton *button, gpointer data);
