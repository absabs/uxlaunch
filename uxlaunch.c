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
#include <signal.h>

#include "uxlaunch.h"


int main(int argc, char **argv)
{
	open_log();

	/*
	 * General objective:
	 * Do the things that need root privs first,
	 * then switch to the final user ASAP.
	 *
	 * Once we're at the target user ID, we need
	 * to start X since that's the critical element
	 * from that point on.
	 *
	 * While X is starting, we can do the things
	 * that we need to do as the user UID, but that
	 * don't need X running yet.
	 *
	 * We then wait for X to signal that it's ready
	 * to draw stuff.
	 *
	 * Once X is running, we set up the ConsoleKit session,
	 * check if the screensaver needs to lock the screen
	 * and then start the window manager.
	 * After that we go over the autostart .desktop files
	 * to launch the various autostart processes....
	 * ... and we're done.
	 */

	get_options(argc, argv);
	set_tty();

	setup_pam_session();

	setup_xauth();

	switch_to_user();


	start_X_server();

	/*
	 * These steps don't need X running
	 * so can happen while X is talking to the
	 * hardware
	 */
	start_ssh_agent();

	wait_for_X_signal();

	setup_consolekit_session();

	/* dbus needs the CK env var */
	start_dbus_session_bus();
	start_gconf();

	maybe_start_screensaver();

	start_desktop_session();

	autostart_panels();
	autostart_desktop_files();
	do_autostart();

	wait_for_X_exit();

	set_text_mode();

	// close_consolekit_session();
	stop_ssh_agent();
	stop_dbus_session_bus();
	close_pam_session();

	/* Make sure that we clean up after ourselves */
	sleep(2);
	kill(0, SIGKILL);

	return EXIT_SUCCESS;
}
