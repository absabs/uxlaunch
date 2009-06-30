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
#include <string.h>
#include <getopt.h>
#include <dirent.h>
#include <pwd.h>

#include "uxlaunch.h"

/* builtin defaults */
int tty = 2;
char session[256] = "/usr/bin/metacity --sm-disable";
char username[256] = "moblin";


static struct option opts[] = {
	{ "user", 1, NULL, 'u' },
	{ "tty", 1, NULL, 't' },
	{ "session", 1, NULL, 's' },
	{ "help", 1, NULL, 'h' },
	{ 0, 0, NULL, 0}
};


void usage(const char *name)
{
	printf("Usage: %s [OPTION...] [-- [session cmd] [session args]]\n", name);
	printf("  -u, --user      Start session as specific username\n");
	printf("  -t, --tty       Start session on alternative tty number\n");
	printf("  -s, --session   Start a non-default session\n");
	printf("  -h, --help      Display this help message\n");
}

void get_options(int argc, char **argv)
{
	int i = 0;
	int c;
	FILE *f;
	DIR *dir;
	struct dirent *entry;
	char msg[256];

	/*
	 * default fallbacks are listed above in the declations
	 * each step below overrides them in order
	 */

	/* try and find a user in /home/ */
	dir = opendir("/home");
	while (dir) {
		char buf[80];
		char *u;
		struct passwd *p;

		entry = readdir(dir);
		if (!entry)
			break;
		if (entry->d_name[0] == '.')
			continue;
		if (strcmp(entry->d_name, "lost+found")==0)
			continue;
		if (entry->d_type != DT_DIR)
			continue;

		u = strdup(entry->d_name);
		/* check if this is actually a valid user */
		p = getpwnam(u);
		if (!p)
			continue;
		/* and make sure this is actually the guys homedir */
		snprintf(buf, 80, "/home/%s", u);
		if (!strcmp(p->pw_dir, buf))
			continue;
		strncpy(username, u, 256);
	}
	if (dir)
		closedir(dir);

	/* parse config file */
	f = fopen("/etc/sysconfig/uxlaunch", "r");
	if (f) {
		char buf[256];
		char *key;
		char *val;

		while (fgets(buf, 80, f) != NULL) {

			key = strtok(buf, "=");
			if (!key)
				continue;
			val = strtok(NULL, "=");
			if (!val)
				continue;

			if (!strcmp(key, "user"))
				strncpy(username, val, 256);
			if (!strcmp(key, "tty"))
				tty = atoi(val);
			if (!strcmp(key, "session"))
				strncpy(session, val, 256);
		}
		fclose(f);
	}

	/* parse cmdline - overrides */
	while (1) {
		c = getopt_long(argc, argv, "u:t:s:h", opts, &i);
		if (c == -1)
			break;

		switch (c) {
		case 'u':
			strncpy(username, optarg, 256);
			break;
		case 't':
			tty = atoi(optarg);
			break;
		case 's':
			strncpy(session, optarg, 256);
			break;
		case 'h':
			usage(argv[0]);
			exit (EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}

	/* Get session command from startup line */
	while (i < argc) {
		if (!strcmp(argv[i++], "--")) {
			session[0] = '\0';
			while (i < argc) {
				strncat(session, argv[i++], 256 - strlen(session));
				if (i != argc)
					strncat(session, " ", 256 - strlen(session));
			}
		}
	}

	log_string("options:");
	snprintf(msg, 256, "username: %s", username);
	log_string(msg);
	snprintf(msg, 256, "tty %d", tty);
	log_string(msg);
	snprintf(msg, 256, "session: %s", session);
	log_string(msg);

	pass = getpwnam(username);
	if (!pass)
		exit(EXIT_FAILURE);


}
