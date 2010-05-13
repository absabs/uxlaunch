/*
 * uxlaunch.c: desktop session starter
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
#include <pwd.h>

#include "uxlaunch.h"


int main(int argc, char **argv)
{
//	char xhost_cmd[80];

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

	setup_xauth();

//	setup_efs();

	set_tty();

//	start_oom_task();

//	setup_pam_session();

	switch_to_user();

	/*
	 * BUG: udev is sometimes not done when we go and start Xorg,
	 * which results in the mouse/kbd not working. To work around this
	 * we call udevadm settle and force the system to wait for
	 * device probing to complete, which may be a long time
	 */
	if (system("/sbin/udevadm settle --timeout 10") != EXIT_SUCCESS)
		lprintf("udevadm settle bash returned an error");

	start_X_server();

	/*
	 * These steps don't need X running
	 * so can happen while X is talking to the
	 * hardware
	 */

	wait_for_X_signal();

//	start_ssh_agent();

//	setup_consolekit_session();

	/* dbus needs the CK env var */
	start_dbus_session_bus();

	/* gconf needs dbus */
	start_gconf();

//	maybe_start_screensaver();

	start_desktop_session();

	/* finally, set local username to be allowed at any time,
	 * which is not depenedent on hostname changes */
//	snprintf(xhost_cmd, 80, "/usr/bin/xhost +SI:localuser:%s",
//		 pass->pw_name);
//	if (system(xhost_cmd) != 0)
//		lprintf("%s failed", xhost_cmd);

//	get_session_type();
//	autostart_desktop_files();
//	do_autostart();

	/*
	 * we do this now to make sure dbus etc are not spawning
	 * dbus-activated
	 * tasks at non-oomkillable priorities
	 */

//	oom_adj(xpid, -17);
//	oom_adj(session_pid, -17);
//	oom_adj(getpid(), -17);

	/*
	 * The desktop session runs here
	 */
	wait_for_X_exit();

	set_text_mode();

	// close_consolekit_session();
//	stop_ssh_agent();
	stop_dbus_session_bus();
//	close_pam_session();
//	stop_oom_task();

	/* Make sure that we clean up after ourselves */
	sleep(2);

	lprintf("Terminating uxlaunch and all children");
	kill(0, SIGKILL);

	return EXIT_SUCCESS;
}
