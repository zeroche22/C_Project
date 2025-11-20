#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "analysis.h"
#include "server.h"

#define TCP_PORT 8080
#define BUFFER_SIZE 1024
#define LINE_BUFFER 2048
#define DATA_DIR "data"

static int ensure_data_directory(void) {
    struct stat st;
    if (stat(DATA_DIR, &st) == -1) {
        if (mkdir(DATA_DIR, 0755) == -1) {
            perror("mkdir data");
            return -1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s exists but is not a directory\n", DATA_DIR);
        return -1;
    }
    return 0;
}

static int append_line_to_file(const char *path, const char *line) {
    FILE *fp = fopen(path, "a");
    if (!fp) {
        perror("fopen append");
        return -1;
    }
    fputs(line, fp);
    fputc('\n', fp);
    fclose(fp);
    return 0;
}

static char *extract_json_value(const char *json, const char *key) {
    if (!json || !key) {
        return NULL;
    }

    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *key_pos = strstr(json, pattern);
    if (!key_pos) {
        return NULL;
    }

    const char *colon = strchr(key_pos + strlen(pattern), ':');
    if (!colon) {
        return NULL;
    }

    const char *cursor = colon + 1;
    while (*cursor == ' ' || *cursor == '\t') {
        cursor++;
    }

    if (*cursor != '"') {
        return NULL;
    }
    cursor++;

    const char *start = cursor;
    while (*cursor && *cursor != '"') {
        if (*cursor == '\\' && *(cursor + 1) != '\0') {
            cursor += 2;
            continue;
        }
        cursor++;
    }

    if (*cursor != '"') {
        return NULL;
    }

    size_t len = (size_t)(cursor - start);
    char *value = malloc(len + 1);
    if (!value) {
        return NULL;
    }

    size_t out = 0;
    for (const char *p = start; p < start + len; ++p) {
        if (*p == '\\' && (p + 1) < (start + len)) {
            ++p;
            switch (*p) {
                case '\"':
                    value[out++] = '"';
                    break;
                case '\\':
                    value[out++] = '\\';
                    break;
                case 'n':
                    value[out++] = '\n';
                    break;
                case 't':
                    value[out++] = '\t';
                    break;
                default:
                    value[out++] = *p;
                    break;
            }
        } else {
            value[out++] = *p;
        }
    }

    value[out] = '\0';
    return value;
}

static void handle_line(const char *line, const char *client_ip) {
    if (!line || !client_ip) {
        return;
    }

    if (ensure_data_directory() != 0) {
        return;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.json", DATA_DIR, client_ip);
    append_line_to_file(filepath, line);

    char *message = extract_json_value(line, "message");
    if (message) {
        save_match_context(client_ip, message);
        free(message);
    }
}

int start_tcp_server(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(TCP_PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("TCP server listening on port %d\n", TCP_PORT);

    char line_buffer[LINE_BUFFER];
    size_t line_len = 0;
    char recv_buffer[BUFFER_SIZE];

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }

        char ip_str[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
        printf("Accepted connection from %s\n", ip_str);

        line_len = 0;
        ssize_t bytes_read;
        while ((bytes_read = read(client_fd, recv_buffer, sizeof(recv_buffer))) > 0) {
            for (ssize_t i = 0; i < bytes_read; ++i) {
                char ch = recv_buffer[i];
                if (ch == '\n') {
                    line_buffer[line_len] = '\0';
                    handle_line(line_buffer, ip_str);
                    line_len = 0;
                } else if (line_len + 1 < sizeof(line_buffer)) {
                    line_buffer[line_len++] = ch;
                }
            }
        }

        if (line_len > 0) {
            line_buffer[line_len] = '\0';
            handle_line(line_buffer, ip_str);
            line_len = 0;
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
