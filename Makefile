VERSION = 0.51

CC := gcc

all: uxlaunch

install: uxlaunch
	mkdir -p $(DESTDIR)/usr/sbin \
	         $(DESTDIR)/etc/sysconfig/ \
	         $(DESTDIR)/usr/share/man/man1/
	install -m0755  uxlaunch $(DESTDIR)/usr/sbin/
	[ -f $(DESTDIR)/etc/sysconfig/uxlaunch ] || \
	    install -m0644 uxlaunch.sysconfig $(DESTDIR)/etc/sysconfig/uxlaunch
	install -m0644 uxlaunch.1 $(DESTDIR)/usr/share/man/man1/uxlaunch.1

OBJS := uxlaunch.o desktop.o dbus.o misc.o user.o xserver.o \
	lib.o options.o 

CFLAGS += -Wall -W -Os -g -fstack-protector -D_FORTIFY_SOURCE=2 -Wformat -fno-common \
	 -Wimplicit-function-declaration  -Wimplicit-int \
	`pkg-config --cflags dbus-1` \
	-D VERSION=\"$(VERSION)\"

LDADD  += `pkg-config --libs dbus-1` \
	  -lpthread -lrt -lXau

%.o: %.c uxlaunch.h Makefile
	@echo "  CC  $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

uxlaunch: $(OBJS) Makefile
	@echo "  LD  $@"
	@$(CC) -o $@ $(OBJS) $(LDADD) $(LDFLAGS)

clean:
	rm -rf *.o *~ uxlaunch

dist:
	git tag v$(VERSION)
	git archive --format=tar --prefix="uxlaunch-$(VERSION)/" v$(VERSION) | \
		gzip > uxlaunch-$(VERSION).tar.gz
