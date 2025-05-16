CC = gcc
CFLAGS = -std=c99 -Iinclude -Wall -Wextra
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=src/%.o)
BIN = bin/program.exe

# Create bin directory if it doesn't exist (Windows compatible)
all: $(BIN)

$(BIN): create_bin $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(filter-out create_bin,$^)

create_bin:
	@if not exist bin mkdir bin

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist src\*.o del src\*.o
	@if exist $(BIN) del $(BIN)

# Additional targets for debugging
debug: CFLAGS += -g -DDEBUG
debug: $(BIN)

.PHONY: all clean debug create_bin