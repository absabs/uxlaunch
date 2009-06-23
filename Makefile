all: uxlaunch

OBJS := uxlaunch.o consolekit.o  dbus.o  desktop.o  misc.o  pam.o user.o xserver.o lib.o

CFLAGS += -Wall -W `pkg-config --cflags dbus-1`
LDADD  += `pkg-config --libs dbus-1`

uxlaunch: $(OBJS)
	gcc -o uxlaunch $(OBJS) $(LDADD)


clean:
	rm -rf *.o *~ uxlaunch