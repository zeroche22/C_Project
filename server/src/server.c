#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#include "server.h"
#include "storage.h"

#define DEFAULT_PORT "8080"
#define BASE_DIR "data/clients"

struct client_info {
    int sock;
    char ip[INET_ADDRSTRLEN];
};

void handle_client(int client_sock, const char *client_ip);

static void *client_thread(void *arg) {
    struct client_info *info = (struct client_info *)arg;
    int sock = info->sock;
    char ip[INET_ADDRSTRLEN];
    strncpy(ip, info->ip, sizeof(ip));
    ip[sizeof(ip) - 1] = '\0';
    free(info);

    printf("[+] Client connected: %s\n", ip);
    handle_client(sock, ip);
    printf("[-] Client disconnected: %s\n", ip);
    return NULL;
}

void run_server(const char *port_str) {
    int port = atoi(port_str);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port: %s\n", port_str);
        exit(EXIT_FAILURE);
    }

    if (storage_init_base(BASE_DIR) != 0) {
        fprintf(stderr, "Failed to init storage base dir\n");
        exit(EXIT_FAILURE);
    }

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons((uint16_t)port);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 16) < 0) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    printf("server listening on port %d\n", port);

    for (;;) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        struct client_info *info = malloc(sizeof(*info));
        if (!info) {
            perror("malloc");
            close(client_fd);
            continue;
        }

        info->sock = client_fd;
        if (!inet_ntop(AF_INET, &cli_addr.sin_addr, info->ip, sizeof(info->ip))) {
            perror("inet_ntop");
            close(client_fd);
            free(info);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, info) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(info);
            continue;
        }
        pthread_detach(tid);
    }

    close(listen_fd);
}

int main(int argc, char *argv[]) {
    const char *port = (argc > 1) ? argv[1] : DEFAULT_PORT;
    run_server(port);
    return 0;
}
