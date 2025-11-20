CC = gcc
CFLAGS = -std=c99 -Iinclude
SRC = src/cJSON.c

all: main_keylogger

main_keylogger: main/main_keylogger.c $(SRC)
	$(CC) $(CFLAGS) main/main_keylogger.c $(SRC) -o main/main_keylogger

clean:
	rm -f main/main_keylogger