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

#include <dbus/dbus.h>
#include <ck-connector.h>

static CkConnector *connector = NULL;


void setup_consolekit_session(void)
{
	DBusError error;

	log_string("Entering setup_consolekit_session");

	connector = ck_connector_new();
	if (!connector)
		exit(1);

	dbus_error_init(&error);
	// FIXME - open session with parameters instead
	if (!ck_connector_open_session(connector, &error)) {
		printf("Error: Unable to open session with ConsoleKit: %s: %s\n",
			error.name, error.message);
		exit(1);
	}

	setenv("XDG_SESSION_COOKIE", ck_connector_get_cookie(connector), 1);

	log_environment();
}


void close_consolekit_session(void)
{
	log_string("Entering close_consolekit_session");

	DBusError error;

	dbus_error_init(&error);
	if (connector)
		ck_connector_close_session(connector, &error);

	unsetenv("XDG_SESSION_COOKIE");
}
