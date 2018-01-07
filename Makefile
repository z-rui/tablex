CC=gcc
CFLAGS=-Wall -ggdb3 -pipe
CFLAGS+=`pkg-config --cflags ncursesw glib-2.0 ibus-1.0`

VALAC=valac-0.34
VALACFLAGS=--save-temps --pkg=glib-2.0 --pkg=ibus-1.0

# Debug
#VALACFLAGS+=--debug -X -ggdb3 

# Optimization
CFLAGS+=-O2 -flto
LDFLAGS+=-O2 -flto
VALACFLAGS+=-X -O2 -X -flto

all: curses ibus

curses: tst.o dict.o edit.o main.o
	$(CC) -o $@ $(LDFLAGS) tst.o dict.o edit.o main.o \
		`pkg-config --libs ncursesw`

ibus: ibus.o tst.o dict.o edit.o
	$(CC) -o $@ $(LDFLAGS) tst.o dict.o edit.o ibus.o \
		`pkg-config --libs glib-2.0 ibus-1.0`

ibus.o: ibus.vala
	$(VALAC) -c $(VALACFLAGS) ibus.vala config.vapi edit.vapi

.PHONY: all
