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

#include "uxlaunch.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "uxlaunch.h"

struct spawn_record {
	char command[8192];
	int flags;
};

static void *spawn(void *ptr)
{
	static int counter = 0;
	struct spawn_record *spw = ptr;

	if (spw->flags & PIN) {
		cpu_set_t cpu_set;
		CPU_ZERO(&cpu_set);
		CPU_SET(1, &cpu_set);
		sched_setaffinity(0, CPU_SETSIZE, &cpu_set);
	}
	if (spw->flags & NICE) {
		nice(19);
	}
	if (spw->flags & DELAYED) {
		/* todo: make this flexible to wait for CPU idle */
		counter++;
		sleep(10 + counter);
	}

	system(spw->command);
	free(spw);
	return NULL;
}

void start_daemon(int flags, char *cmd, char *args)
{
	struct spawn_record *spw;
	pthread_t threadid;
	char msg[80];

	/*
	 * If the command does not exist... return right away
	 */
	if (access(cmd, X_OK))
		return;

	sprintf(msg, "Starting %s", cmd);

	/* DELAYED implies BACKGROUND */
	if (flags & DELAYED)
		flags |= BACKGROUND;

	spw = malloc(sizeof(struct spawn_record));
	if (!spw)
		return;

	sprintf(spw->command, "%s", cmd);
	if (args)
		sprintf(spw->command,"%s %s", cmd, args);
	spw->flags = flags;

	pthread_create(&threadid, NULL, spawn, spw);

	if (!(flags & BACKGROUND))
		pthread_join(threadid, NULL);
}
