CC=gcc
CFLAGS=-Wall -ggdb3 -pipe
CFLAGS+=`pkg-config --cflags ncursesw glib-2.0 ibus-1.0`

VALAC=valac-0.36
VALACFLAGS=--save-temps --pkg=glib-2.0 --pkg=ibus-1.0

# Debug
#VALACFLAGS+=--debug -X -ggdb3

# Optimization
CFLAGS+=-O2 -flto
LDFLAGS+=-O2 -flto
VALACFLAGS+=-X -O2 -X -flto

all: curses ibus graph

curses: tst.o dict.o edit.o main.o
	$(CC) -o $@ $(LDFLAGS) $^ \
		`pkg-config --libs ncursesw`

ibus: ibus.o tst.o dict.o edit.o
	$(CC) -o $@ $(LDFLAGS) $^ \
		`pkg-config --libs glib-2.0 ibus-1.0`

graph: graph.o tst.o dict.o edit.o
	$(CC) -o $@ $(LDFLAGS) $^

ibus.o: ibus.vala config.vapi edit.vapi
	$(VALAC) -c $(VALACFLAGS) $^

clean:
	rm -f *.o

.PHONY: all clean
