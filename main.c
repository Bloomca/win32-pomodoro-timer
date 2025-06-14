#include <windows.h>
#include <dwmapi.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL IsSystemDarkMode();

BOOL isDarkMode;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {

    isDarkMode = IsSystemDarkMode();

    MSG msg;
    WNDCLASSW wc = {0};

    wc.lpszClassName = L"Timer App";
    wc.hInstance = hInstance;
    wc.hbrBackground = NULL;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClass(&wc);
    CreateWindow(wc.lpszClassName, L"Timer app", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 330, 270, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));

            int backdropType = 2; // DWMSBT_MAINWINDOW (Mica)
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
            break;
        // case WM_PAINT: {
        //     // need to not paint the default background
        //     // so the Mica background shows up
        //     PAINTSTRUCT ps;
        //     HDC hdc = BeginPaint(hwnd, &ps);
        //     EndPaint(hwnd, &ps);
        //     return 0;
        // }
        // case WM_ERASEBKGND:
        //     // Return non-zero to prevent default background erasing
        //     return 1;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

BOOL IsSystemDarkMode() {
    HKEY hKey;
    BOOL darkMode = FALSE;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD value = 0;
        DWORD size = sizeof(DWORD);
        if (RegQueryValueEx(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            darkMode = (value == 0);
        }
        RegCloseKey(hKey);
    }
    return darkMode;
}