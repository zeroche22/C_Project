#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "storage.h"

char *g_base_dir = NULL;

int mkdir_p(const char *path) {
    char *tmp = strdup(path);
    if (!tmp) return -1;

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }

    int result = (mkdir(tmp, 0755) == 0 || errno == EEXIST) ? 0 : -1;
    free(tmp);
    return result;
}

int storage_init_base(const char *base_dir) {
    if (!base_dir) return -1;

    g_base_dir = strdup(base_dir);
    if (!g_base_dir) return -1;

    return mkdir_p(g_base_dir);
}

void storage_cleanup(void) {
    if (g_base_dir) {
        free(g_base_dir);
        g_base_dir = NULL;
    }
}

char *build_path(const char *ip, const char *folder, const char *filename) {
    size_t size = strlen(g_base_dir) +
                  strlen(ip) +
                  strlen(folder) +
                  strlen(filename) + 4;

    char *path = malloc(size);
    if (!path) return NULL;

    snprintf(path, size, "%s/%s/%s/%s", g_base_dir, ip, folder, filename);

    return path;
}

int storage_ensure_client_dirs(const char *ip) {
    char *raw_path = build_path(ip, "raw", "");
    char *analysis_path = build_path(ip, "analysis", "");

    if (!raw_path || !analysis_path) {
        free(raw_path);
        free(analysis_path);
        return -1;
    }

    int r1 = mkdir_p(raw_path);
    int r2 = mkdir_p(analysis_path);

    free(raw_path);
    free(analysis_path);

    return (r1 == 0 && r2 == 0) ? 0 : -1;
}

int storage_get_next_index(const char *ip) {
    char *path = build_path(ip, "raw", "");
    if (!path) return 1;

    DIR *dir = opendir(path);
    if (!dir) {
        free(path);
        return 1;
    }

    int max = 0;
    struct dirent *ent;

    while ((ent = readdir(dir))) {
        int idx;
        if (sscanf(ent->d_name, "%d.json", &idx) == 1) {
            if (idx > max)
                max = idx;
        }
    }

    closedir(dir);
    free(path);

    return max + 1;
}

int write_file(const char *path, const char *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;

    fwrite(data, 1, len, f);
    fclose(f);
    return 0;
}

int storage_save_raw(const char *ip, int index, const char *data, size_t len) {

    char name[32];
    snprintf(name, sizeof(name), "%04d.json", index);

    char *path = build_path(ip, "raw", name);
    if (!path) return -1;

    int res = write_file(path, data, len);
    free(path);

    return res;
}

int storage_save_analysis(const char *ip, int index, const char *data, size_t len) {

    char name[32];
    snprintf(name, sizeof(name), "%04d_result.json", index);

    char *path = build_path(ip, "analysis", name);
    if (!path) return -1;

    int res = write_file(path, data, len);
    free(path);

    return res;
}
