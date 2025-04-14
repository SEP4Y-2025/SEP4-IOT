CC = gcc
CFLAGS = -Wall -I/usr/local/include
LDFLAGS = -lmosquitto -lcjson

SRC = src/main.c
OBJ = $(SRC:.c=.o)
TARGET = iot

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
