#include <string.h>
#include <stdio.h>

#include "key_decoder.h"

static void append_key(char *buffer, size_t *len, size_t max_len,
                       const char *key) {

    if (strcmp(key, "SPACE") == 0) {
        if (*len + 1 < max_len)
            buffer[(*len)++] = ' ';
    }
    else if (strcmp(key, "ENTER") == 0) {
        if (*len + 1 < max_len)
            buffer[(*len)++] = '\n';
    }
    else if (strcmp(key, "BACKSPACE") == 0) {
        if (*len > 0)
            (*len)--;
    }
    else {
        size_t klen = strlen(key);
        if (*len + klen < max_len) {
            memcpy(&buffer[*len], key, klen);
            *len += klen;
        }
    }

    buffer[*len] = '\0';
}

void extract_text_from_json(const char *json, char *out, size_t out_size) {
    if (!json || !out || out_size == 0)
        return;

    const char *p = json;
    size_t len = 0;
    out[0] = '\0';

    while ((p = strstr(p, "\"key\"")) != NULL) {
        p = strchr(p, ':');
        if (!p) break;
        p++;

        while (*p == ' ' || *p == '\"')
            p++;

        char key[32];
        int k = 0;

        while (*p && *p != '"' && k < (int)sizeof(key) - 1)
            key[k++] = *p++;

        key[k] = '\0';

        append_key(out, &len, out_size, key);
    }
}
