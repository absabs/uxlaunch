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
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "uxlaunch.h"


#define LOGFILE "/var/log/uxlaunch.log"

extern char **environ;

static int first_time = 1;
static int logfile_enabled = 1;

struct timeval start;

static FILE *log;


void open_log(void)
{
	/* truncate log */
	log = fopen(LOGFILE, "w");
	if (!log)
		logfile_enabled = 0;
}

void log_string(char *string)
{
	struct timeval current;
	uint64_t secs, usecs;
	char msg[8192];

	if (first_time) {
		first_time = 0;
		gettimeofday(&start, NULL);
	}

	gettimeofday(&current, NULL);

	secs = current.tv_sec - start.tv_sec;
	
	while (current.tv_usec < start.tv_usec) {
		secs --;
		current.tv_usec += 1000000;
	}
	usecs = current.tv_usec - start.tv_usec;

	snprintf(msg, 8192, "[%02llu.%06llu] [%d] %s", secs, usecs, getpid(), string);
	if (msg[strlen(msg)] != '\n')
		strcat(msg, "\n");
	if (verbose)
		fprintf(stderr, "%s", msg);

	if (!logfile_enabled)
		return;

	if (log) {
		fputs(msg, log);
		fflush(log);
	} else {
		logfile_enabled = 0;
		log_string("Unable to write logfile");
	}
}


void log_environment(void)
{
	char **env;

	env = environ;
	
	log_string("Dumping environment");
	log_string("----------------------------------");
	while (*env) {
		log_string(*env);
		env++;
	}
	log_string("----------------------------------");
}

