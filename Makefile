all: uxlaunch

OBJS := uxlaunch.o consolekit.o dbus.o desktop.o misc.o pam.o user.o xserver.o lib.o

<<<<<<< HEAD:Makefile
CFLAGS += -Wall -W -Os -g -fstack-protector -D_FORTIFY_SOURCE=2 -Wformat \
	  `pkg-config --cflags dbus-1` \
	  `pkg-config --cflags ck-connector`
LDADD  += `pkg-config --libs dbus-1` \
	  `pkg-config --libs ck-connector` \
	  -lpam

uxlaunch: $(OBJS) Makefile
	gcc -o uxlaunch $(OBJS) $(LDADD)


clean:
	rm -rf *.o *~ uxlaunch
