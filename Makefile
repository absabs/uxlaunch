all: uxlaunch

OBJS := uxlaunch.o consolekit.o dbus.o desktop.o misc.o pam.o user.o xserver.o lib.o

CFLAGS += -Wall -W -Os -g -fstack-protector -D_FORTIFY_SOURCE=2 -Wformat -fno-common \
	`pkg-config --cflags dbus-1` \
	`pkg-config --cflags ck-connector`

LDADD  += `pkg-config --libs dbus-1` \
	  `pkg-config --libs ck-connector` \
	  -lpam -lpthread

uxlaunch: $(OBJS) Makefile
	gcc -o uxlaunch $(OBJS) $(LDADD)

.SILENT:

clean:
	rm -rf *.o *~ uxlaunch
