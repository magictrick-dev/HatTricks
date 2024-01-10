#ifndef MAIN_H
#define MAIN_H
#include <windows.h>

// --- User-defined Messages ---------------------------------------------------
//
// Windows allows us to create our own user-defined messages starting at the
// WM_USER constant up to 0x7FFF. We are reserving two new messages called
// UD_CREATE_WINDOW and UD_DESTROY_WINDOW which use to instantiate windows.
//

#define UD_CREATE_WINDOW    (WM_USER + 0x0042)
#define UD_DESTROY_WINDOW   (WM_USER + 0x0043)

// --- Window Creation Context -------------------------------------------------
//
// The window creation context structure basically packs the parameters of
// CreateWindowExW so that we can pass off for creation.
//

struct win32_window_creation_context
{
    DWORD       ex_style;
    LPCWSTR     class_name;
    LPCWSTR     window_name;
    DWORD       style;
    int         x;
    int         y;
    int         width;
    int         height;
    HWND        parent_window;
    HMENU       menu;
    HINSTANCE   instance;
    LPVOID      user_parameter;
};

#endif
