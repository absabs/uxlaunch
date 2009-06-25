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
#include <pwd.h>

#include <security/pam_appl.h>

#include "uxlaunch.h"

pam_handle_t *ph;
struct pam_conv pc;

/*
 * Creating a PAM session. We need a pam "login" session so that the dbus
 * "at_console" logic will work correctly, as well as various /dev file
 * permissions.
 * 
 * for pam_console to work we need to set the PAM_TTY and PAM_XDISPLAY variables,
 * before we open the session. "PAM_TTY" takes input in the form "ttyX", without
 * the /dev prefix, so we need to construct that in place here.
 */
void setup_pam_session(void)
{
	char msg[256];
	char x[256];
	int err;

	log_string("Entering setup_pam_session");

	snprintf(x, 256, "tty%d", vtnum);

	err = pam_start("login", pass->pw_name, &pc, &ph);

	err = pam_set_item(ph, PAM_TTY, &x);
	if (err != PAM_SUCCESS) {
		snprintf(msg, 255, "pam_set_item PAM_TTY returned %d: %s\n", err, pam_strerror(ph, err));
		log_string(msg);
		exit(EXIT_FAILURE);
	}

	err = pam_set_item(ph, PAM_XDISPLAY, &displayname);
	if (err != PAM_SUCCESS) {
		snprintf(msg, 255, "pam_set_item PAM_DISPLAY returned %d: %s\n", err, pam_strerror(ph, err));
		log_string(msg);
		exit(EXIT_FAILURE);
	}

	err = pam_open_session(ph, 0);
	if (err != PAM_SUCCESS) {
		snprintf(msg, 255, "pam_open_session returned %d: %s\n", err, pam_strerror(ph, err));
		log_string(msg);
		exit(EXIT_FAILURE);
	}
}

void close_pam_session(void)
{
	char msg[256];
	int err;

	log_string("Entering close_pam_session");

	err = pam_close_session(ph, 0);
	if (err) {
		snprintf(msg, 255, "pam_close_session returned %d: %s\n", err, pam_strerror(ph, err));
		log_string(msg);
	}
	pam_end(ph, err);
}
