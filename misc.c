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

void start_ssh_agent(void)
{
	log_string("** Entering start_ssh_agent");
}

void start_bash(void)
{
	int ret;

	log_string("Entering start_bash");

	fprintf(stderr, "Starting bash shell -- type exit to continue\n");
	ret = system("/bin/bash");
	if (ret != EXIT_SUCCESS)
		log_string("bash returned an error");
}

void start_gconf(void)
{
	log_string("** Entering start_gconf");
}


void maybe_start_screensaver(void)
{
	log_string("** Entering maybe_start_screensaver");
}

