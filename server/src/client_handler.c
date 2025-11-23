#include "key_decoder.h"
#include "analysis.h"
#include "storage.h"

void handle_client(int client_sock, const char *ip) {
    storage_ensure_client_dirs(ip);

    int index = storage_get_next_index(ip);
    char buffer[8192];
    int len = recv(client_sock, buffer, sizeof(buffer)-1, 0);
    if (len <= 0) return;

    buffer[len] = '\0';

    storage_save_raw(ip, index, buffer, len);

    char text[8192];
    extract_text_from_json(buffer, text);

    analyze_text_and_store(ip, index, text);
}
