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
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "uxlaunch.h"

#include <X11/Xauth.h>

int uid;
struct passwd *pass;

//char user_xauth_path[PATH_MAX];

static void do_env(void)
{
	char buf[PATH_MAX];
	FILE *file;
	/* start with a clean environ */
	clearenv();

	setenv("USER", pass->pw_name, 1);
	setenv("LOGNAME", pass->pw_name, 1);
	setenv("HOME", pass->pw_dir, 1);
	setenv("SHELL", pass->pw_shell, 1);
	snprintf(buf, PATH_MAX, "/var/spool/mail/%s", pass->pw_name);
	setenv("MAIL", buf, 1);
	setenv("DISPLAY", displayname, 1);
	snprintf(buf, PATH_MAX, "/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:%s/bin", pass->pw_dir);
	setenv("PATH", buf, 1);
//	snprintf(user_xauth_path, PATH_MAX, "%s/.Xauthority", pass->pw_dir);
	setenv("XAUTHORITY", xauth_cookie_file, 1);
/*	snprintf(buf, PATH_MAX, "%s/.cache", pass->pw_dir);
	mkdir(buf, 0700);
	setenv("XDG_CACHE_HOME", buf, 0);
	snprintf(buf, PATH_MAX, "%s/.config", pass->pw_dir);
	setenv("XDG_CONFIG_HOME", buf, 0);
	setenv("OOO_FORCE_DESKTOP","gnome", 0);
	setenv("LIBC_FATAL_STDERR_", "1", 0);*/

	file = popen("/bin/bash -l -c export", "r");
	if (!file)
		return;

	while (!feof(file)) {
		char *c;
		memset(buf, 0, sizeof(buf));
		if (fgets(buf, sizeof(buf) - 1, file) == NULL)
				break;
		c = strchr(buf, '\n');

		if (strlen(buf) < 12)
			continue;
		if (c)
			*c = 0;

		if (strstr(buf, "PWD"))
			continue;
//		if (strstr(buf, "DISPLAY"))
//			continue;

		c = strchr(buf, '=');
		if (c) {
			char *c2;
			*c = 0;
			c++;
			if (*c == '"') c++;
			c2 = strchr(c, '"');
			if (c2)
				*c2 = 0;
			lprintf("Setting %s to %s\n", &buf[11], c);
			setenv(&buf[11], c, 1);
		}

	}

	pclose(file);
}


/*
 * Change from root (as we started) to the target user.
 * Steps
 * 1) setuid/getgid
 * 2) env variables: HOME, MAIL, LOGNAME, USER, SHELL, DISPLAY and PATH
 * 3) chdir(/home/foo);
 */
void switch_to_user(void)
{
	int ret;

	lprintf("Entering switch_to_user");

	initgroups(pass->pw_name, pass->pw_gid);

	/* make sure that the user owns /dev/ttyX */
	ret = chown(displaydev, pass->pw_uid, pass->pw_gid);
	if (ret)
		lprintf("Failed to fix /dev/tty permission");

	if (!((setgid(pass->pw_gid) == 0) && (setuid(pass->pw_uid) == 0)))
		exit(EXIT_FAILURE);

	setsid();

	do_env();

	if (chdir(pass->pw_dir))
		lprintf("Failed to chdir");
}
