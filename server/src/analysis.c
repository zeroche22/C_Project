#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analysis.h"

#define DATA_DIR "data"

static char *escape_json(const char *text) {
    size_t len = strlen(text);
    size_t capacity = len * 2 + 1;
    char *escaped = malloc(capacity);
    if (!escaped) {
        return NULL;
    }

    size_t out = 0;
    for (size_t i = 0; i < len; ++i) {
        char ch = text[i];
        switch (ch) {
            case '\\':
            case '"':
                escaped[out++] = '\\';
                escaped[out++] = ch;
                break;
            case '\n':
                escaped[out++] = '\\';
                escaped[out++] = 'n';
                break;
            case '\r':
                escaped[out++] = '\\';
                escaped[out++] = 'r';
                break;
            case '\t':
                escaped[out++] = '\\';
                escaped[out++] = 't';
                break;
            default:
                escaped[out++] = ch;
                break;
        }
    }

    escaped[out] = '\0';
    return escaped;
}

char *extract_context(const char *message, size_t position, size_t radius) {
    if (!message) {
        return NULL;
    }

    size_t len = strlen(message);
    if (position > len) {
        return NULL;
    }

    size_t start = (position > radius) ? position - radius : 0;
    size_t end = position + radius;
    if (end > len) {
        end = len;
    }

    size_t out_len = end - start + 1; // include char at position
    char *context = malloc(out_len + 1);
    if (!context) {
        return NULL;
    }

    memcpy(context, message + start, out_len);
    context[out_len] = '\0';
    return context;
}

void save_match_context(const char *ip, const char *message) {
    if (!ip || !message) {
        return;
    }

    size_t len = strlen(message);
    for (size_t i = 0; i < len; ++i) {
        if (message[i] != '@') {
            continue;
        }

        char *context = extract_context(message, i, 20);
        if (!context) {
            continue;
        }

        char *escaped_context = escape_json(context);
        char *escaped_message = escape_json(message);
        if (!escaped_context || !escaped_message) {
            free(context);
            free(escaped_context);
            free(escaped_message);
            continue;
        }

        char path[256];
        snprintf(path, sizeof(path), "%s/%s_matches.json", DATA_DIR, ip);
        FILE *fp = fopen(path, "a");
        if (!fp) {
            perror("fopen match file");
            free(context);
            free(escaped_context);
            free(escaped_message);
            continue;
        }

        fprintf(fp, "{\"index\":%zu,\"context\":\"%s\",\"message\":\"%s\"}\n", i, escaped_context, escaped_message);
        fclose(fp);

        free(context);
        free(escaped_context);
        free(escaped_message);
    }
}
