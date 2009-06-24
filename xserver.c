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
#include <string.h>
#include <signal.h>

#include "uxlaunch.h"

char displaydev[256];		/* "/dev/tty1" */
char displayname[256];		/* ":0" */
int vtnum;	 		/* number part after /dev/tty */
char xauth_cookie_file[256];    /* including an --auth prefix */

void find_display_and_tty(void)
{
	int len;
	char msg[256];

	log_string("Entering find_display_and_tty");

	len = readlink("/proc/self/fd/0", displaydev, sizeof(displaydev) - 1);
	if (len != -1)
		displaydev[len] = '\0';

	vtnum = atoi(displaydev + 8);

	sprintf(msg, "tty = %s", displaydev);
	log_string(msg);
	sprintf(msg, "vtnum = %d", vtnum);
	log_string(msg);
}

void setup_xauth(void)
{
	FILE *fp;
	char cookie[16];
	char msg[80];
	unsigned int i;

	log_string("** Entering setup_xauth");

	fp = fopen("/dev/urandom", "r");
	if (!fp)
		return;
	if (fgets(cookie, sizeof(cookie), fp) == NULL)
		return;
	sprintf(msg, "cookie = ");
	for (i = 0; i < sizeof(cookie); i++) {
		char c[256];
		sprintf(c, "%02x", (unsigned char)cookie[i]);
		strcat(msg, c);
	}
	log_string(msg);
}

static void usr1handler(int foo)
{
	/* Got the signal from the X server that it's ready */
	if (foo++) foo--;
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
	char *xserver = NULL;
	int ret;
	char vt[80];

	log_string("Entering start_X_server");

	/* Step 1: arm the signal */

	memset(&act, 0, sizeof(struct sigaction));

	act.sa_handler = usr1handler;
	sigaction(SIGUSR1, &act, NULL);

	/* Step 2: fork */

	ret = fork();
	if (!ret)
		return; /* we're the main thread */

	/* if we get here we're the child */

	/* Step 3: find the X server */

	if (!xserver && !access("/usr/bin/Xorg", X_OK))
		xserver = "/usr/bin/Xorg";
	if (!xserver && !access("/usr/bin/X", X_OK))
		xserver = "/usr/bin/X";
	if (!xserver) {
		log_string("No X server found!");
		return;
	}

	sprintf(vt, "vt%d", vtnum);

	/* Step 4: start the X server */
	execl(xserver, xserver,  displayname, "-nr", "-verbose", xauth_cookie_file,
	      "-nolisten", "-tcp", vt, NULL);
}

void wait_for_X_signal(void)
{
	log_string("** Entering wait_for_X_signal");
}
 
