#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>

int storage_init_base(const char *base_dir);
void storage_cleanup(void);
int storage_ensure_client_dirs(const char *ip);
int storage_get_next_index(const char *ip);
int storage_save_raw(const char *ip, int index, const char *data, size_t len);
int storage_save_analysis(const char *ip, int index, const char *data, size_t len);

#endif
