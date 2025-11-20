CC       ?= gcc
CFLAGS   ?= -Wall -Wextra -std=c11 -O2
CPPFLAGS ?= -Iinclude

SERVER_OBJS = src/server.c src/analysis.c src/tcp_main.c
HTTP_OBJS   = src/http.c src/analysis.c src/http_main.c

all: tcp_server http_server

tcp_server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

http_server: $(HTTP_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

clean:
	rm -f tcp_server http_server

.PHONY: all clean
