/**
 ******************************************************************************
 * @file 	threads.c
 * @author 	Can Gulmez
 * @brief 	Additional threads of passive acoustic surveillance.
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

/**
 * Read the associated device node to get the payload data. This
 * is done by creating a new thread because of attempting to not
 * freezing the main thread.
 */
void *payload_data_thread(void *arg)
{
	int s;
	int deviceFd = *((int *)arg);

	while (payloadThreadStarted)
	{
		/* Read the device node to get the payload data. */
		read_device_node(deviceFd);

		/* Take the payload mutex to update the UI. */
		s = pthread_mutex_lock(&payloadMutex);
		if (s != 0)
			syscall_error();

		/* Update the UI data. */
		update_mic_data();
		update_nav_data();
		update_gps_data();

		/* Give up the payload mutex. */
		s = pthread_mutex_unlock(&payloadMutex);
		if (s != 0)
			syscall_error();

		/* Take the another required actions. */
		// record_payload_data();
	}
	return NULL;
}
 