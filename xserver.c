/*
 * This file is part of uxlaunch
 *
 * (C) Copyright 2009 Intel Corporation
 * Authors: 
 *     Auke Kok <auke@linux.intel.com>
 *     Arjan van de Ven <arjan@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "uxlaunch.h"


void find_display_and_tty(void)
{
	int len;
	char msg[256];

	log_string("Entering find_display_and_tty");

	len = readlink("/proc/self/fd/0", *displaydev, sizeof(displaydev) - 1);
	if (len != -1)
		displaydev[len] = '\0';

	sprintf(msg, "tty = %s", *displaydev);
	log_string(msg);

	log_string("Leaving find_display_and_tty");
}

void start_X_server(void)
{
	log_string("Entering start_X_server");
	log_string("Leaving start_X_server");
}

void wait_for_X_signal(void)
{
	log_string("Entering wait_for_X_signal");
	log_string("Leaving wait_for_X_signal");
}
 
