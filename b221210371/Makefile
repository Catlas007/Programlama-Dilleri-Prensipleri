#Makefile for Space Travel Simulation
# Ensures MinGW compatibility
CC = gcc
CFLAGS = -std=c99 -Iinclude -Wall -Wextra
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=src/%.o)
BIN = bin/program.exe

# Create bin directory if it doesn't exist (Windows compatible)
all: create_bin $(BIN)

create_bin:
	@if not exist bin mkdir bin

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist src\*.o del src\*.o
	@if exist $(BIN) del $(BIN)

# Additional targets for debugging
debug: CFLAGS += -g -DDEBUG
debug: create_bin $(BIN)

.PHONY: all clean debug create_bin