CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -pthread
LDFLAGS = -pthread

SRC = \
src/server.c \
src/client_handler.c \
src/storage.c \
src/analysis.c \
src/key_decoder.c

OBJ = $(SRC:.c=.o)

TARGET = json_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
