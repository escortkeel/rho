-include ../Makefile.local

CC= gcc

CCFLAGS= -Wall -O3 -std=gnu11 -I../lib/glfw/include

SRCS=	main.c render.c sim.c

LIBS=	../lib/glfw/src/libglfw3.a -ldl -lm -lpthread `pkg-config --cflags --libs gl x11 xrandr xi xxf86vm xinerama xcursor`

.PHONY: all clean

all: rho

rho: $(SRCS)
	$(CC) $(CCFLAGS) $(SRCS) $(LIBS) -o rho

clean:
	rm -f rho
