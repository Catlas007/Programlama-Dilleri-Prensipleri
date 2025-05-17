
CC = gcc
CFLAGS = -std=c99 -Iinclude -Wall -Wextra


SRCDIR = src
LIBDIR = lib  
BINDIR = bin
INCDIR = include


BIN = $(BINDIR)/Test


OBJECTS = $(LIBDIR)/kisi.o $(LIBDIR)/uzayAraci.o $(LIBDIR)/zaman.o $(LIBDIR)/gezegen.o \
          $(LIBDIR)/dosyaOkuma.o $(LIBDIR)/buzDeviGezegen.o $(LIBDIR)/cuceGezegen.o \
          $(LIBDIR)/gazDeviGezegen.o $(LIBDIR)/kayacGezegen.o $(LIBDIR)/simulasyon.o \
          $(LIBDIR)/main.o


dirs:
	@if not exist bin mkdir bin
	@if not exist lib mkdir lib


all: dirs derle bagla


derle: dirs
	$(CC) $(CFLAGS) -o $(LIBDIR)/kisi.o -c $(SRCDIR)/kisi.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/uzayAraci.o -c $(SRCDIR)/uzayAraci.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/zaman.o -c $(SRCDIR)/zaman.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/gezegen.o -c $(SRCDIR)/gezegen.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/dosyaOkuma.o -c $(SRCDIR)/dosyaOkuma.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/buzDeviGezegen.o -c $(SRCDIR)/buzDeviGezegen.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/cuceGezegen.o -c $(SRCDIR)/cuceGezegen.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/gazDeviGezegen.o -c $(SRCDIR)/gazDeviGezegen.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/kayacGezegen.o -c $(SRCDIR)/kayacGezegen.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/simulasyon.o -c $(SRCDIR)/simulasyon.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/main.o -c $(SRCDIR)/main.c


bagla: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $(LIBDIR)/uzayAraci.o $(LIBDIR)/zaman.o $(LIBDIR)/kisi.o $(LIBDIR)/gezegen.o \
	$(LIBDIR)/kayacGezegen.o $(LIBDIR)/buzDeviGezegen.o $(LIBDIR)/gazDeviGezegen.o $(LIBDIR)/cuceGezegen.o $(LIBDIR)/dosyaOkuma.o $(SRCDIR)/simulasyon.c


calistir: $(BIN)
	$(BIN)


clean:
	@if exist lib\*.o del lib\*.o
	@if exist $(BIN) del $(BIN)


debug: CFLAGS += -g -DDEBUG
debug: all

.PHONY: all derle bagla calistir clean debug dirs
