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
	int deviceFd = *((int *)arg);

	while (micThreadStarted)
	{
		/* Read the device node to get the payload data. */
		read_device_node(deviceFd);

		/* Take the various required actions. */
		update_mic_data();
		update_nav_data();
		update_gps_data();	
		record_payload_data();
	}
	return NULL;
}
