#include <windows.h>
#include <dwmapi.h>

#define BTN_TIMER_START 1001
#define BTN_TIMER_STOP 1002

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
    CreateWindow(wc.lpszClassName, L"Timer app", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 400, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hBtnStart, hBtnStop;
    static HFONT hFont;

    switch (msg) {
        case WM_CREATE:
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));

            int backdropType = 2; // DWMSBT_MAINWINDOW (Mica)
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

            MARGINS margins = {-1, -1, -1, -1};
            DwmExtendFrameIntoClientArea(hwnd, &margins);

            HWND hStatic = CreateWindowW(L"Static", L"25:00", WS_CHILD | WS_VISIBLE | SS_LEFT,
                20, 20, 150, 60, 
                hwnd, (HMENU) 1, NULL, NULL);

            HFONT hFont = CreateFont(
                48,                        // Larger size
                0, 0, 0,
                FW_BOLD,                   // Bold weight
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,         // ClearType rendering
                DEFAULT_PITCH | FF_SWISS,
                L"Segoe UI"
            );

            SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);

            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Fill with black (will be transparent due to DWM)
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            
            SetBkMode(hdc, TRANSPARENT);
            
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            HWND hwndStatic = (HWND)lParam;
            
            // 1 is static controls
            if (GetDlgCtrlID(hwndStatic) == 1)
            {
                SetTextColor(hdcStatic, RGB(220, 220, 220));
                SetBkMode(hdcStatic, TRANSPARENT);
                return (LRESULT)GetStockObject(NULL_BRUSH);
            }
            break;
        }

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
        DWORD size = sizeof(DWORD);

        DWORD lightTheme = 1;
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, 
                        (LPBYTE)&lightTheme, &size);
        darkMode = (lightTheme == 0) ? TRUE : FALSE;

        if (darkMode == TRUE) {
            return darkMode;
        }

        DWORD value = 1;
        RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, NULL, 
                        (LPBYTE)&value, &size);
        darkMode = (value == 0) ? 1 : 0;

        RegCloseKey(hKey);
    }

    return darkMode;
}