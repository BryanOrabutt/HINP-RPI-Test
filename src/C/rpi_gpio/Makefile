clean:
	$(info Removing old binary...)
	rm hinp_rpi.o

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe `pkg-config --cflags gtk+-3.0`
	
hinp_rpi.o: hinp_rpi.c hinp_rpi.h
	gcc -g -c $(CCFLAGS) -Wall -o hinp_rpi.o hinp_rpi.c
