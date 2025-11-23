#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "analysis.h"
#include "storage.h"

#define CONTEXT_SIZE 20

void analyze_text_and_store(const char *ip, int index, const char *text) {
    if (!ip || !text)
        return;

    size_t len = strlen(text);

    char *result = malloc(len * 2);
    if (!result)
        return;

    result[0] = '\0';
    strcat(result, "{ \"matches\": [");

    int found = 0;

    for (size_t i = 0; i < len; i++) {
        if (text[i] == '@') {
            int start = (i > CONTEXT_SIZE) ? (i - CONTEXT_SIZE) : 0;
            int end = (i + CONTEXT_SIZE < len) ? (i + CONTEXT_SIZE) : len - 1;

            char snippet[64];
            int s = 0;

            for (int j = start; j <= end && s < 60; j++) {
                char c = text[j];
                if (c == '\n') c = ' ';
                snippet[s++] = c;
            }
            snippet[s] = '\0';

            if (found > 0)
                strcat(result, ", ");

            strcat(result, "\"");
            strcat(result, snippet);
            strcat(result, "\"");

            found++;
        }
    }

    char tail[128];
    snprintf(tail, sizeof(tail),
             "], \"total_matches\": %d }\n", found);

    strcat(result, tail);

    storage_save_analysis(ip, index, result, strlen(result));

    free(result);
}
