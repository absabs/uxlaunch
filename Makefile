VERSION = 0.7

CC := gcc

all: uxlaunch

install: uxlaunch
	mkdir -p $(DESTDIR)/usr/sbin
	install uxlaunch $(DESTDIR)/usr/sbin/

OBJS := uxlaunch.o consolekit.o dbus.o desktop.o misc.o pam.o user.o xserver.o \
	lib.o options.o

CFLAGS += -Wall -W -Os -g -fstack-protector -D_FORTIFY_SOURCE=2 -Wformat -fno-common \
	 -Wimplicit-function-declaration  -Wimplicit-int \
	`pkg-config --cflags dbus-1` \
	`pkg-config --cflags ck-connector` \
	-D VERSION=\"$(VERSION)\"

LDADD  += `pkg-config --libs dbus-1` \
	  `pkg-config --libs ck-connector` \
	  -lpam -lpthread -lrt -lXau

uxlaunch: $(OBJS) Makefile
	$(CC) -o uxlaunch $(OBJS) $(LDADD) $(LDFLAGS)

.SILENT:

clean:
	rm -rf *.o *~ uxlaunch

dist:
	git tag v$(VERSION)
	git archive --format=tar -v --prefix="uxlaunch-$(VERSION)/" v$(VERSION) | \
		gzip > uxlaunch-$(VERSION).tar.gz
