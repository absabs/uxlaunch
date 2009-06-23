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

/*
 * This function needs to find the user name and UID/GID of the user
 * we want to be logged in as.
 * Stragegy:
 * Pick the first one from
 * 1) passed as --user <name> on the command line
 * 2) from /etc/sysconfig/uxlaunch
 * 3) first user in /home
 * 4) "moblin"
 */

void find_user(int argc, char **argv)
{
	log_string("Entering find_user");
	log_string("Leaving find_user");
}

/*
 * Change from root (as we started) to the target user.
 * Steps
 * 1) setuid/getgid
 * 2) env variables: HOME, MAIL, LOGNAME
 * 3) chdir(/home/foo);
 */
void switch_to_user(void)
{
	log_string("Entering switch_to_user");
	log_string("Leaving switch_to_user");
}

