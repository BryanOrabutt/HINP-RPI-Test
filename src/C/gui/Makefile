# change application name here (executable output name)
TARGET=cfd_gui

# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe  -I/usr/include/gtk-3.0/ \
    -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ \
     -I/usr/include/pango-1.0/ -I/usr/include/cairo/ \
	-I/usr/include/gdk-pixbuf-2.0/ -I/usr/include/atk-1.0/ \
	-I../rpi_gpio/

#GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# linker
LD=gcc
#  LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic -rdynamic
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic

OBJS=    $(TARGET).o cfd.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

cfd_gui.o: cfd_gui.c
	$(CC) -c $(CCFLAGS) cfd_gui.c $(GTKLIB) -o cfd_gui.o

clean:
	rm -f *.o $(TARGET)
