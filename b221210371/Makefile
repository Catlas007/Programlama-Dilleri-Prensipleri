# Makefile for Space Travel Simulation
# Ensures MinGW compatibility
CC = gcc
CFLAGS = -std=c99 -Iinclude -Wall -Wextra
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BIN = bin/program

# Create bin directory if it doesn't exist
$(shell mkdir -p bin)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(BIN)

# Additional targets for debugging
debug: CFLAGS += -g -DDEBUG
debug: $(BIN)

.PHONY: all clean debug