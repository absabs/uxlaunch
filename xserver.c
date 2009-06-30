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

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <linux/tiocl.h>
#include <linux/limits.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <pwd.h>

#include "uxlaunch.h"

#include <X11/Xauth.h>

char displaydev[PATH_MAX];	/* "/dev/tty1" */
char displayname[256] = ":0";	/* ":0" */
int vtnum;	 		/* number part after /dev/tty */
char xauth_cookie_file[PATH_MAX];
Xauth x_auth;

static pthread_mutex_t notify_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t notify_condition = PTHREAD_COND_INITIALIZER;

static int xpid;

#define XAUTH_DIR "/var/run/uxlaunch"

/*
 * We need to know the DISPLAY and TTY values to use, for passing
 * to PAM, ConsoleKit but also X.
 * TIOCLINUX will tell us which console is currently showing
 * for this purpose.
 */
void set_tty(void)
{
	int fd;
	char msg[256];
	unsigned char tiocl_sub = TIOCL_GETFGCONSOLE;

	log_string("Entering set_tty");

	/* switch to this console */
	fd = open("/dev/console", O_RDWR);
	if (fd < 0) {
		log_string("Unable to open /dev/console, using stdin");
		fd = 0;
	}
	ioctl(fd, VT_ACTIVATE, tty);
	if (fd != 0)
		close(fd);

	/*
	 * In a later version we need to be able to get the
	 * terminal to start on from a command line parameter
	 */

	fd = open("/dev/console", O_RDWR);
	if (fd < 0) {
		log_string("Unable to open /dev/console, using stdin");
		fd = 0;
	}
	vtnum = ioctl(fd, TIOCLINUX, &tiocl_sub);
	if (fd != 0)
		close(fd);

	if (vtnum < 0) {
		log_string("TIOCL_GETFGCONSOLE failed");
		exit(EXIT_FAILURE);
	}

	/* kernel starts counting at 0 */
	vtnum++;

	snprintf(displaydev, PATH_MAX, "/dev/tty%d", vtnum);

	snprintf(msg, 256, "Using %s (vt%d) as display device", displaydev, vtnum);
	log_string(msg);
}

void setup_xauth(void)
{
	FILE *fp;
	int fd;
	static char cookie[16];
	char msg[80];
	unsigned int i;
	struct utsname uts;

	static char xau_address[80];
	static char xau_number[] = "0"; // FIXME, detect correct displaynum
	static char xau_name[] = "MIT-MAGIC-COOKIE-1";

	log_string("Entering setup_xauth");

	fp = fopen("/dev/urandom", "r");
	if (!fp)
		return;
	if (fgets(cookie, sizeof(cookie), fp) == NULL)
		return;
	fclose(fp);

	snprintf(msg, 80, "cookie = ");
	for (i = 0; i < sizeof(cookie); i++) {
		char c[256];
		snprintf(c, 256, "%02x", (unsigned char)cookie[i]);
		strcat(msg, c);
	}
	log_string(msg);

	/* construct xauth data */
	if (uname(&uts) < 0) {
		log_string("uname failed");
		return;
	}

	sprintf(xau_address, "%s", uts.nodename);
	x_auth.family = FamilyLocal;
	x_auth.address = xau_address;
	x_auth.number = xau_number;
	x_auth.name = xau_name;
	x_auth.address_length = strlen(xau_address);
	x_auth.number_length = strlen(xau_number);
	x_auth.name_length = strlen(xau_name);
	x_auth.data = (char *) cookie;
	x_auth.data_length = sizeof(cookie);


	mkdir(XAUTH_DIR, 01755);
	snprintf(xauth_cookie_file, PATH_MAX, "%s/Xauth-%s-XXXXXX", XAUTH_DIR, pass->pw_name);

	fd = mkstemp(xauth_cookie_file);
	if (fd <= 0) {
		log_string("unable to make tmp file for xauth");
		return;
	}

	log_string(xauth_cookie_file);

	fp = fdopen(fd, "a");
	if (!fp) {
		log_string("unable to open xauth fp");
		close(fd);
		return;
	}

	/* write it out to disk */
	if (XauWriteAuth(fp, &x_auth) != 1)
		log_string("unable to write xauth data to disk");

	fclose(fp);
}

static void usr1handler(int foo)
{
	/* Got the signal from the X server that it's ready */
	if (foo++) foo--; /*  shut down warning */

	pthread_mutex_lock(&notify_mutex);
	pthread_cond_signal(&notify_condition);
	pthread_mutex_unlock(&notify_mutex);
}


static volatile int exiting = 0;

static void termhandler(int foo)
{
	if (foo++) foo--; /*  shut down warning */

	exiting = 1;
	/*
	 * we received either:
	 * - a TERM from init when switching to init 3
	 * - an INT from a ^C press in the console when running in fg
	 */
	kill(xpid, SIGKILL);
}

/*
 * start the X server
 * Step 1: arm the signal
 * Step 2: fork to get ready for the exec, continue from the main thread
 * Step 3: find the X server
 * Step 4: start the X server
 */
void start_X_server(void)
{
	struct sigaction usr1;
	struct sigaction term;
	char *xserver = NULL;
	int ret;
	char vt[80];

	log_string("Entering start_X_server");

	/* Step 1: arm the signal */
	memset(&usr1, 0, sizeof(struct sigaction));
	usr1.sa_handler = usr1handler;
	sigaction(SIGUSR1, &usr1, NULL);

	/* Step 2: fork */
	ret = fork();
	if (ret) {
		xpid = ret;
		/* setup sighandler for main thread */
		memset(&term, 0, sizeof(struct sigaction));
		term.sa_handler = termhandler;
		sigaction(SIGTERM, &term, NULL);
		sigaction(SIGINT, &term, NULL);
		return; /* we're the main thread */
	}

	/* if we get here we're the child */

	/* Step 3: find the X server */

	/*
	 * set the X server sigchld to SIG_IGN, that's the
         * magic to make X send the parent the signal.
	 */
	signal(SIGUSR1, SIG_IGN);

	if (!xserver && !access("/usr/bin/Xorg", X_OK))
		xserver = "/usr/bin/Xorg";
	if (!xserver && !access("/usr/bin/X", X_OK))
		xserver = "/usr/bin/X";
	if (!xserver) {
		log_string("No X server found!");
		_exit(EXIT_FAILURE);
	}

	snprintf(vt, 80, "vt%d", vtnum);

	/* Step 4: start the X server */
	execl(xserver, xserver,  displayname, "-nr", "-verbose", "-auth", xauth_cookie_file,
	      "-nolisten", "tcp", vt, NULL);
	exit(0);
}

/*
 * The X server will send us a SIGUSR1 when it's ready to serve clients,
 * wait for this.
 */
void wait_for_X_signal(void)
{
	struct timespec tv;
	log_string("Entering wait_for_X_signal");
	clock_gettime(CLOCK_REALTIME, &tv);
	tv.tv_sec += 10;

	pthread_mutex_lock(&notify_mutex);
	pthread_cond_timedwait(&notify_condition, &notify_mutex, &tv);
	pthread_mutex_unlock(&notify_mutex);
	log_string("done");

}

void wait_for_X_exit(void)
{	
	int ret;
	log_string("wait_for_X_exit");
	while (!exiting) {
		ret = waitpid(0, NULL, 0);
		if (ret == xpid)
			break;
	}
	log_string("X exited");
}

void set_text_mode(void)
{
	int fd;

	log_string("Setting console mode to KD_TEXT");

	fd = open(displaydev, O_RDWR);

	if (fd < 0) {
		log_string("Unable to open /dev/console, using stdin");
		fd = 0;
	}
	ioctl(fd, KDSETMODE, KD_TEXT);
	if (fd != 0)
		close(fd);

}
