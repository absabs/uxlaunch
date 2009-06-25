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

#include "uxlaunch.h"

/*
 * Process a .desktop file
 * Objective: fine the "Exec=" line which has the command to run
 * Constraints:
 *   if there is a OnlyShowIn line, then it must contain the string "MOBLIN"
 *   (this is to allow moblin-only settings apps to show up, but not gnome settings apps)
 *   if there is a NotShowIn line, it must not contain "MOBLIN" or "GNOME"
 *   (this allows KDE etc systems to hide stuff for GNOME as they do today and not show it
 *    on moblin)
 */
static void do_desktop_file(const char *filename)
{
	FILE *file;
	char line[4096];
	char exec[4096];
	int show = 1;

	file = fopen(filename, "r");
	if (!file)
		return;

	memset(exec, 0, 4096);
	while (!feof(file)) {
		char *c;
		if (fgets(line, 4096, file) == NULL)
			break;
		c = strchr(line, '\n');
		if (c) *c = 0;	
		if (strstr(line, "Exec="))
			strncpy(exec, line+5, 4095);


		if (strstr(line, "OnlyShowIn"))
			if (strstr(line, "MOBLIN") == NULL)
				show = 0;

		if (strstr(line, "NotShowIn")) {
			if (strstr(line, "MOBLIN"))
				show = 0;
			if (strstr(line, "GNOME"))
				show = 0;
		}
		
	}
	fclose(file);
	if (show && strlen(exec)>0) {
		char msg[4096];
		snprintf(msg, 4096, "Starting -%s-", exec);
		log_string(msg);
		/* FIXME: split the arguments and do an execlp or so instead */
		if (!fork()) {
			system(exec);
			exit(0);
		}
	}
}

/*
 * We need to process all the .desktop files in /etc/xdg/autostart.
 * Simply walk the directory 
 */
void autostart_desktop_files(void)
{
	DIR *dir;
	struct dirent *entry;

	log_string("Entering autostart_desktop_files");

	dir = opendir("/etc/xdg/autostart");
	if (!dir) {
		log_string("Autostart directory not found");
		return;
	}	

	while (1) {
		char filename[PATH_MAX];
		entry = readdir(dir);
		if (!entry)
			break;
		if (entry->d_name[0] == '.')
			continue;
		if (entry->d_type != DT_REG)
			continue;
		if (strchr(entry->d_name, '~')) 
			continue;  /* editor backup file */
		snprintf(filename, 4096, "/etc/xdg/autostart/%s", entry->d_name);
		do_desktop_file(filename);
	}
	closedir(dir);
}

void start_metacity(void)
{
	int ret;
	log_string("Entering start_metacity");

	ret = system("/usr/bin/metacity");
	if (ret != EXIT_SUCCESS) 
		log_string("Failure to start metacity");
}
