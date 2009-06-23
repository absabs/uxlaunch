all: uxlaunch

OBJS := uxlaunch.o consolekit.o dbus.o desktop.o misc.o pam.o user.o xserver.o lib.o

CFLAGS += -Wall -W `pkg-config --cflags dbus-1` -Os -g -fstack-protector -D_FORTIFY_SOURCE=2 -Wformat
LDADD  += `pkg-config --libs dbus-1` -lpam

uxlaunch: $(OBJS) Makefile
	gcc -o uxlaunch $(OBJS) $(LDADD)


clean:
	rm -rf *.o *~ uxlaunch
