#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client_handler.h"
#include "storage.h"
#include "key_decoder.h"
#include "analysis.h"

#define BUF_SIZE 10000

void handle_client(int client_sock, const char *ip) {
    storage_ensure_client_dirs(ip);

    while (1) {
        char buffer[BUF_SIZE];
        int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);

        if (len <= 0) {
            break;
        }

        buffer[len] = '\0';

        int index = storage_get_next_index(ip);
        storage_save_raw(ip, index, buffer, len);

        char text[BUF_SIZE];
        extract_text_from_json(buffer, text, sizeof(text));
        analyze_text_and_store(ip, index, text);
    }
    
    close(client_sock);
}