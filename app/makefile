CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = ../lib/libqtc.so -lm
SRC = src/main.c
BIN = bin/codec

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

clean:
	rm -rf bin/*
