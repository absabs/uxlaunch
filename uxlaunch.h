#ifndef __INCLUDE_GUARD_UXLAUNCH_
#define __INCLUDE_GUARD_UXLAUNCH_


extern void find_user(void);
extern void setup_pam_session(void);
extern void switch_to_user(void);
extern void find_display_and_tty(void);
extern void start_X_server(void);
extern void wait_for_X_signal(void);
extern void start_dbus_session_bus(void);
extern void setup_consolekit_session(void);
extern void start_ssh_agent(void);
extern void start_gconf(void);
extern void maybe_start_screensaver(void);
extern void start_metacity(void);
extern void autostart_desktop_files(void);


extern void log_string(char *string);
extern void log_environment(void);

extern void close_log(void);

#endif
