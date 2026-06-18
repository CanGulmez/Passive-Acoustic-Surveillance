/**
 ******************************************************************************
 * @file 	timeouts.c
 * @author 	Can Gulmez
 * @brief 	Timeout utilities of passive acoustic surveillance.
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
 * Set the timeout to get the Keras logs into text view.
 */
gboolean model_keras_log_timeout(gpointer data)
{
	(void)data;

	const char *kerasLog;

	/* Get the current text view and then update it. */
	modelTextBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(modelTextView));
	kerasLog = get_keras_script_log(MODEL_LOG_PATH);
	print_log("fetched the keras script log from related file");

	gtk_text_buffer_set_text(modelTextBuffer, kerasLog, -1);
	print_log("printed the keras script log within text view");

	if (is_keras_script_running(modelFitPid))
	{
		return G_SOURCE_CONTINUE;
	}
	else
	{
		return G_SOURCE_REMOVE;
	}
}
