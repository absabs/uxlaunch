all: uxlaunch

install: uxlaunch
	mkdir -p $(DESTDIR)/usr/sbin
	install -m0755 -o root -g root uxlaunch $(DESTDIR)/usr/sbin/

OBJS := uxlaunch.o consolekit.o dbus.o desktop.o misc.o pam.o user.o xserver.o lib.o

CFLAGS += -Wall -W -Os -g -fstack-protector -D_FORTIFY_SOURCE=2 -Wformat -fno-common \
	 -Wimplicit-function-declaration  -Wimplicit-int \
	`pkg-config --cflags dbus-1` \
	`pkg-config --cflags ck-connector`

LDADD  += `pkg-config --libs dbus-1` \
	  `pkg-config --libs ck-connector` \
	  -lpam -lpthread -lrt -lXau

uxlaunch: $(OBJS) Makefile
	gcc -o uxlaunch $(OBJS) $(LDADD)

.SILENT:

clean:
	rm -rf *.o *~ uxlaunch
