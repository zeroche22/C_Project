#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <stddef.h>

char *extract_context(const char *message, size_t position, size_t radius);
void save_match_context(const char *ip, const char *message);

#endif // ANALYSIS_H
