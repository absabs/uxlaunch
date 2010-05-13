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


/*
 * We want to start gconf early, by hand, so that it can start processing the
 * XML well before someone needs it to cut down the total time
 */
void start_gconf(void)
{
	int ret;

	ret = system("gconftool-2 --spawn");
	if (ret)
		lprintf("failed to start gconftool-2: %d", ret);
}
