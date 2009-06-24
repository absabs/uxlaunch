#ifndef __INCLUDE_GUARD_UXLAUNCH_
#define __INCLUDE_GUARD_UXLAUNCH_


/*
 * Target user information
 */
extern struct passwd *pass;
extern char *user;
extern int   uid;

extern char displaydev[];
extern char displayname[];
extern int  vtnum;

extern void find_user(int argc, char **argv);
extern void setup_pam_session(void);
extern void close_pam_session(void);
extern void switch_to_user(void);
extern void find_tty(void);
extern void setup_xauth(void);
extern void start_X_server(void);
extern void wait_for_X_signal(void);
extern void start_dbus_session_bus(void);
extern void stop_dbus_session_bus(void);
extern void setup_consolekit_session(void);
extern void start_ssh_agent(void);
extern void start_gconf(void);
extern void maybe_start_screensaver(void);
extern void start_metacity(void);
extern void autostart_desktop_files(void);
extern void start_bash(void);


extern void log_string(char *string);
extern void log_environment(void);

extern void close_log(void);

#define NORMAL 0
#define NICE 1
#define PIN 2
#define DELAYED 4
#define BACKGROUND 8

extern void start_daemon(int flags, char *cmd, char *args);

#endif
