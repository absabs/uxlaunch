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

char displaydev[256];		/* "/dev/tty1" */
char displayname[256];		/* ":0" */


void find_display_and_tty(void)
{
	int len;
	char msg[256];

	log_string("Entering find_display_and_tty");

	len = readlink("/proc/self/fd/0", displaydev, sizeof(displaydev) - 1);
	if (len != -1)
		displaydev[len] = '\0';

	sprintf(msg, "tty = %s", displaydev);
	log_string(msg);
}

static void use1handler(int foo)
{
	/* Got the signal from the X server that it's ready */
}

/*
 * start the X server
 * Step 1: arm the signal
 * Step 2: fork to get ready for the exec, continue from the main thread
 * Step 3: find the X server
 * Step 4: start the X server
 */
void start_X_server(void)
{
	struct sigaction act;

	log_string("** Entering start_X_server");

	memset(&act, 0, sizeof(struct sigaction));

	act.sa_handler = usr1handler;
	sigaction(SIGUSR1, &act, NULL);

	ret = fork();
	if (!ret)
		return; /* we're the main thread */

	
}

void wait_for_X_signal(void)
{
	log_string("** Entering wait_for_X_signal");
}
 
