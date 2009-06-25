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

#include "uxlaunch.h"

#include <X11/Xauth.h>

char *user;
int uid;
struct passwd *pass;

static char user_xauth_path[PATH_MAX];


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
	DIR *dir;
	struct dirent *entry;
	int i;
	log_string("Entering find_user");

	/* pass 1: find --user on the command line */
	for (i = 0; i < argc -1 ; i++) {
		if (!user && strcmp(argv[i], "--user") == 0) {
			user = strdup(argv[i+1]);
		}
	}

	/* pass 2: look in the /etc/sysconfig/uxlaunch file */
	if (!user) {
		FILE *file;
		char line[1024];
		file = fopen("/etc/sysconfig/uxlaunch", "r");
		if (file) {
			char *c;
			memset(line, 0, 1024);
			if (fgets(line, 1023, file) == NULL)
				memset(line, 0, 1024);
			fclose(file);
			c = strchr(line, '=');
			if (c) c++;
			if (c && *c && strstr(line, "user ="))
				user = strdup(c);
		}
	}

	/* pass 3: first user in /home */
	if (!user) {
		dir = opendir("/home");
		while (dir) {
			entry = readdir(dir);
			if (!entry)
				break;
			if (entry->d_name[0] == '.')
				continue;
			if (strcmp(entry->d_name, "lost+found")==0)
				continue;
			if (entry->d_type != DT_DIR)
				continue;
			user = strdup(entry->d_name);
			break;	
		}
		if (dir)
			closedir(dir);
	}

	/* pass 4: "moblin" */
	if (!user)
		user = strdup("moblin");

	log_string("user found is:");
	log_string(user);

	/* translate user name to uid */
	pass = getpwnam(user);
	if (pass)
		uid = pass->pw_uid;
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
	char buf[80];
	int result;
	FILE *fp;
	char fn[256];

	log_string("Entering switch_to_user");

	initgroups(pass->pw_name, pass->pw_gid);

	if (!((setgid(pass->pw_gid) == 0) && (setuid(pass->pw_uid) == 0)))
		exit(EXIT_FAILURE);

	setsid();

	/* start with a clean environ */
	clearenv();

	setenv("USER", pass->pw_name, 1);
	setenv("LOGNAME", pass->pw_name, 1);
	setenv("HOME", pass->pw_dir, 1);
	setenv("SHELL", pass->pw_shell, 1);
	snprintf(buf, 80, "/var/spool/mail/%s", pass->pw_name);
	setenv("MAIL", buf, 1);
	setenv("DISPLAY", displayname, 1);
	snprintf(buf, 80, "/usr/local/bin:/bin:/usr/bin:%s/bin", pass->pw_dir);
	setenv("PATH", buf, 1);
	snprintf(user_xauth_path, PATH_MAX, "%s/.Xauthority", pass->pw_dir);
	setenv("XAUTHORITY", user_xauth_path, 1);

	result = chdir(pass->pw_dir);

	fp = fopen(user_xauth_path, "w");
	if (fp) {
		if (XauWriteAuth(fp, &x_auth) != 1) {
			log_string("Unable to write .Xauthority");
		}
		fclose(fp);
	}

	/* redirect further IO to .xsession-errors */
	snprintf(fn, 255, "%s/.xsession-errors", pass->pw_dir);
	fp = fopen(fn, "w");
	if (fp) {
		fclose(fp);
		fp = freopen(fn, "w", stdout);
		fp = freopen(fn, "w", stderr);
	}

	/* Create a new process group */
//	setpgrp();
}

