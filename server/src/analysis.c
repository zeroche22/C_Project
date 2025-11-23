#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "analysis.h"
#include "storage.h"

#define CONTEXT_SIZE     20
#define SNIPPET_BUF      128
#define RESULT_PADDING   512

static void json_escape(const char *src, char *dst, size_t max) {
    size_t j = 0;

    for (size_t i = 0; src[i] && j + 2 < max; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dst[j++] = '\\';
        }
        dst[j++] = src[i];
    }

    dst[j] = '\0';
}

void analyze_text_and_store(const char *ip, int index, const char *text) {
    if (!ip || !text)
        return;

    size_t len = strlen(text);

    size_t result_cap = len * 4 + RESULT_PADDING;
    char *result = calloc(1, result_cap);
    if (!result)
        return;

    strcat(result, "{ \"matches\": [");

    int found = 0;
    size_t last_match = 0;

    for (size_t i = 0; i < len; i++) {

        if (text[i] != '@')
            continue;

        if (found > 0 && i - last_match < 5)
            continue;

        last_match = i;

        size_t start = (i > CONTEXT_SIZE) ? (i - CONTEXT_SIZE) : 0;
        size_t end   = (i + CONTEXT_SIZE < len) ? (i + CONTEXT_SIZE) : (len - 1);

        char snippet[SNIPPET_BUF];
        size_t s = 0;

        for (size_t j = start; j <= end && s < SNIPPET_BUF - 2; j++) {
            char c = text[j];
            if (c == '\n') c = ' ';
            snippet[s++] = c;
        }
        snippet[s] = '\0';

        char escaped[SNIPPET_BUF * 2];
        json_escape(snippet, escaped, sizeof(escaped));

        if (found > 0)
            strcat(result, ", ");

        strcat(result, "\"");
        strcat(result, escaped);
        strcat(result, "\"");

        found++;
    }

    char tail[128];
    snprintf(tail, sizeof(tail),
             "], \"total_matches\": %d }\n", found);

    strcat(result, tail);

    storage_save_analysis(ip, index, result, strlen(result));

    free(result);
}
