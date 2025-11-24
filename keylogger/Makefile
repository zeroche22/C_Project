CC = gcc
CFLAGS = -std=c99 -Iinclude
LDFLAGS = -lws2_32

SRC = src/cJSON.c

all: main_keylogger

main_keylogger: main/main_keylogger.c $(SRC)
	$(CC) $(CFLAGS) main/main_keylogger.c $(SRC) $(LDFLAGS) -o main/main_keylogger.exe

clean:
	rm -f main/main_keylogger.exe
