/*
 * uxlaunch.c: Moblin starter
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


int main(int argc, char **argv)
{
	open_log();

	find_user(argc, argv);
	find_tty();

	setup_pam_session();

	setup_xauth();

	switch_to_user();


	start_X_server();
	wait_for_X_signal();

	start_dbus_session_bus();
	setup_consolekit_session();

	start_ssh_agent();
	start_gconf();

	maybe_start_screensaver();

	autostart_desktop_files();

	start_metacity();

	// FIXME - start wm in foreground, wait for it to die
	// fork .desktop file startup in background thread
	// then when wm dies, close session stuff properly:
	// close_consolekit_session();
	stop_dbus_session_bus();
	close_pam_session();

	return EXIT_SUCCESS; 
}
