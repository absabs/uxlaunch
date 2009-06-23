all: uxlaunch

OBJS := uxlaunch.o
CFLAGS += -Wall -W `pkg-config --cflags dbus-1` -Os -s
LDADD  += `pkg-config --libs dbus-1` -lpam

uxlaunch: $(OBJS)
	gcc -o uxlaunch $(OBJS) $(LDADD)


clean:
	rm -rf *.o *~ uxlaunch
