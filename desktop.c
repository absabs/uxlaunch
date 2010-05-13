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
#include <sys/types.h>
#include <errno.h>

#include "uxlaunch.h"
#if defined(__i386__)
#  define __NR_ioprio_set 289
#elif defined(__x86_64__)
#  define __NR_ioprio_set 251
#elif defined(__arm__)
#  define __NR_ioprio_set 314
#else
#  error "Unsupported arch"
#endif
#define IOPRIO_WHO_PROCESS 1
#define IOPRIO_CLASS_IDLE 3
#define IOPRIO_CLASS_SHIFT 13
#define IOPRIO_IDLE_LOWEST (7 | (IOPRIO_CLASS_IDLE << IOPRIO_CLASS_SHIFT))


int session_pid;

void start_desktop_session(void)
{
	int ret;
	int count = 0;
	char *ptrs[256];

	ret = fork();

	if (ret) {
		session_pid = ret;
		return; /* parent continues */
	}

	lprintf("Entering start_desktop_session");

//	ret = system("/usr/bin/xdg-user-dirs-update");
//	if (ret)
//		lprintf("/usr/bin/xdg-user-dirs-update failed");

	memset(ptrs, 0, sizeof(ptrs));

	ptrs[0] = strtok(session, " \t");
	while (ptrs[count] && count < 255)
		ptrs[++count] = strtok(NULL, " \t");

	ret = execv(ptrs[0], ptrs);

	if (ret != EXIT_SUCCESS)
		lprintf("Failed to start %s: %s", session, strerror(errno));
}
