#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "storage.h"

static char g_base_dir[256] = {0};

int storage_init_base(const char *base_dir) {
    if (!base_dir) return -1;
    strncpy(g_base_dir, base_dir, sizeof(g_base_dir) - 1);
    g_base_dir[sizeof(g_base_dir) - 1] = '\0';

    mkdir("data", 0755);
    mkdir("data/clients", 0755);

    mkdir(g_base_dir, 0755);

    return 0;
}

void storage_cleanup(void) {
}

static int write_file(const char *path, const char *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("fopen");
        return -1;
    }
    size_t written = fwrite(data, 1, len, f);
    fclose(f);
    if (written != len) {
        fprintf(stderr, "short write to %s\n", path);
        return -1;
    }
    return 0;
}

int storage_ensure_client_dirs(const char *ip) {
    if (!ip || !g_base_dir[0]) return -1;

    char path[512];

    snprintf(path, sizeof(path), "%s/%s", g_base_dir, ip);
    mkdir(path, 0755);

    snprintf(path, sizeof(path), "%s/%s/raw", g_base_dir, ip);
    mkdir(path, 0755);

    snprintf(path, sizeof(path), "%s/%s/analysis", g_base_dir, ip);
    mkdir(path, 0755);

    return 0;
}

int storage_get_next_index(const char *ip) {
    if (!ip || !g_base_dir[0]) return 1;

    char path[512];
    snprintf(path, sizeof(path), "%s/%s/raw", g_base_dir, ip);

    DIR *dir = opendir(path);
    if (!dir) {
        return 1;
    }

    int max_idx = 0;
    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;

        int idx = 0;
        if (sscanf(ent->d_name, "%d.json", &idx) == 1) {
            if (idx > max_idx)
                max_idx = idx;
        }
    }

    closedir(dir);
    return max_idx + 1;
}

int storage_save_raw(const char *ip, int index, const char *data, size_t len)
{
    if (!ip || index <= 0 || !data || !g_base_dir[0])
        return -1;

    char path[512];
    snprintf(path, sizeof(path),
             "%s/%s/raw/%04d.json",
             g_base_dir, ip, index);

    return write_file(path, data, len);
}

int storage_save_analysis(const char *ip, int index, const char *data, size_t len)
{
    if (!ip || index <= 0 || !data || !g_base_dir[0])
        return -1;

    char path[512];
    snprintf(path, sizeof(path),
             "%s/%s/analysis/%04d_result.json",
             g_base_dir, ip, index);

    return write_file(path, data, len);
}
