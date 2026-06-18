/**
 ******************************************************************************
 * @file 	main.h
 * @author 	Can Gulmez
 * @brief 	Passive acoustic surveillance primarily designed for drones.
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

#ifndef ACOUSTIC_SURVEILLANCE_H
#define ACOUSTIC_SURVEILLANCE_H

#ifdef __cplusplus
extern "C" {	
#endif

/* Required libaries */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <termios.h>
#include <sys/types.h>
#include <check.h>
#include <shumate/shumate.h>
#include <sqlite3.h>

#include "microphone.h"
#include "model.h"
#include "navigation.h"
#include "gps-map.h"
#include "headers.h"

// #include "../lib/include/alat.h"
#include "../lib/include/dsp.h"

/* Global macro definitions */

#define TIME_FORMAT							"%F %T"
#define BUFFER_SIZE							512
#define INTERPRETER							"/bin/python3"
#define SYSTEM_LOG_PATH						"./logs/system.log"

/* DATA_SIZE is defined as the sample length. The DSP library defines a 
   predefined DATA_SIZE. Firstly undefine and then redefine it. */
#ifdef DATA_SIZE
#undef DATA_SIZE
#define DATA_SIZE							1024
#endif

#define DB_SENSOR_PATH						"./db/sensor-data.db"
#define DB_SENSOR_TABLE						"SensorData"
#define DB_SQL_LENGTH						30720

#define TIMEOUT_DEVICE_READ					3000	/* ms */
#define TIMEOUT_MODEL_LOG					3000	/* ms */
#define TIMEOUT_DATA_RECORD					8000	/* ms */
#define TIMEOUT_NAV_UPDATE					3000	/* ms */ 
#define TIMEOUT_GPS_UPDATE					3000	/* ms */

/* Attribute and built-in macro definitions  */

#define PACKED								__attribute__((packed, aligned(1)))
#define cmp(fstring, sstring) 				(strcmp(fstring, sstring) == 0)

/* Maro function definitions */

#define print_log(msg, ...)													\
do {																		\
	char buffer[BUFFER_SIZE];												\
																			\
	memset(buffer, 0, BUFFER_SIZE);											\
	snprintf(buffer, sizeof(buffer), "[PID=%d][%s] " msg "\n", 				\
			 getpid(), get_time(TIME_FORMAT), ##__VA_ARGS__);				\
	logging(buffer, strlen(buffer));		/* write the logs */			\
	printf("%s", buffer);		/* print the log buffer to "stdout" */		\
} while (0)

#define syscall_error()														\
do {                                      									\
	fprintf(stderr, "\n*** %s (%s::%d in %s()) ***\n", strerror(errno),		\
			  __FILE__, __LINE__, __func__);		   	 					\
	exit(EXIT_FAILURE);	/* exit with failure status */						\
} while (0)

#define custom_error(errmsg, ...) 											\
do {                            											\
	fprintf(stderr, "\n*** " errmsg " (%s::%d in %s()) ***\n",				\
			  ##__VA_ARGS__, __FILE__, __LINE__, __func__);					\
	exit(EXIT_FAILURE);	/* exit with failure status */						\
} while (0)

#define db_error(db)														\
do {																		\
	fprintf(stderr, "\n*** %s (%s::%d in %s()) ***\n",						\
			  sqlite3_errmsg(db), __FILE__, __LINE__, __func__);			\
	exit(EXIT_FAILURE);	/* exit with failure status */						\
} while (0)

/* General enumerations */

typedef enum _CurrentPage 
{
	PAGE_MICROPHONE = 1,
	PAGE_AI_MODEL,
	PAGE_NAVIGATION,
	PAGE_GPS_MAP
} CurrentPage;

/* Global and structures */
typedef struct PACKED _PayloadData
{
	/* microphone sensors payload data */

	int32_t micFilter0[DATA_SIZE];
	int32_t micFilter1[DATA_SIZE];
	int32_t micFilter2[DATA_SIZE];
	int32_t micFilter3[DATA_SIZE];

	/* IMU sensor payload data */

	float imuAccel[3];				/* [x, y, z] m/s^2 */
	float imuGyro[3];				/* [x, y, z] dps */
	float imuTemp;					/* C */

} PayloadData;

/* General shared widgets and variables */

extern CurrentPage currentPage;
extern PayloadData payloadData;

/* Signal analysis shared widgets and variables */

extern DspTime sigSamples[MIC_COUNT];
extern DspTime sigBeamformed;
extern guint sigVolumest;

/* Database function prototypes */

sqlite3 *db_open(const char *dbPath);
void db_create_table(sqlite3 *db);
void db_bind_data(sqlite3 *db);
void db_query_data(sqlite3 *db);
void db_close(sqlite3 *db);

/* Common utility function prototypes */

void logging(const char* buffer, size_t size);
const char *get_time(const char *format);
void set_serial_attrs(int fd, struct termios *tty);
int get_device_nodes(MicChannel channel);
int open_device_node(MicChannel channel, const char *node);
void read_device_node(int fd);
int get_model_datasets(void);
int run_keras_script(const char *script);
void abort_keras_script(int childPid);
int is_keras_script_running(int childPid);
const char *get_keras_script_log(const char *logFile);

/* Timeout utility function prototypes */

gboolean device_node_timeout(gpointer data);
gboolean model_keras_log_timeout(gpointer data);
gboolean db_record_timeout(gpointer data);
gboolean nav_update_timeout(gpointer data);
gboolean gps_update_timeout(gpointer data);

/* Signal analysis function prototypes */

void prepare_samples(void);
double find_dominant_freq(void);
int calculate_arrival(double freq);
DspTime do_beamforming(double arrival);
void make_signal_analysis(const DspTime *beamformed, int arrival);
int select_sector(void);
NavAccel accel_direction(void);
NavGyro gyro_rotation(void);
void update_nav_data(void);
void update_gps_data(void);

/* General signal handler prototypes */

void on_visible_page_changed(GObject *object, GParamSpec *pspec, gpointer data);
void on_header_button_clicked(GtkButton *button, gpointer data);

#ifdef __cplusplus
}
#endif

#endif /* ACOUSTIC_SURVEILLANCE_H */
