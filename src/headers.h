/**
 ******************************************************************************
 * @file 	headers.c
 * @author 	Can Gulmez
 * @brief 	Header button headers of passive acoustic surveillance.
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

/* Global macro definitions */

#define HEADER_SYSTEM_DEVELOPER				"Can Gulmez"
#define HEADER_SYSTEM_NAME					"Passive Acoustic Surveillance"
#define HEADER_SYSTEM_VERSION				"1.0.0"
#define HEADER_SYSTEM_LICENSE				GTK_LICENSE_MIT_X11
#define HEADER_SYSTEM_WEBSITE				"https://github.com/CanGulmez/Passive-Acoustic-Surveillance"
#define HEADER_SYSTEM_ICON					"weather-clear"

/* Header button enumerations */

typedef enum _HeaderButton 
{
	HEADER_BUTTON_NEW = 1,
	HEADER_BUTTON_SAVE_AS,
	HEADER_BUTTON_TRASH,
	HEADER_BUTTON_SYSTEM,
	HEADER_BUTTON_PREFS,
	HEADER_BUTTON_ABOUT
} HeaderButton;

/* Header button shared widgets and variables */

extern HeaderButton headerButton;

/* Header button function prototypes */

void header_system_window(void);
void header_preferences_window(void);
void header_about_me_window(void);
