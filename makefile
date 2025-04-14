CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lmosquitto -lcjson

SRC = $(wildcard src/**/*.c) src/main.c
OBJ = $(SRC:.c=.o)
TARGET = iot

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
