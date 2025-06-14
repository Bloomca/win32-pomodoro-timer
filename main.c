#include <windows.h>
#include <dwmapi.h>

#define ID_BTN_START 1001
#define TIMER_ID 1

static UINT_PTR timerID = 0;
static int remainingSeconds = 25 * 60;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL IsSystemDarkMode();

HWND timerHandle;
HWND buttonHandle;
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

            timerHandle = CreateWindowW(L"Static", L"25:00", WS_CHILD | WS_VISIBLE | SS_LEFT,
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

            SendMessage(timerHandle, WM_SETFONT, (WPARAM)hFont, TRUE);

            buttonHandle = CreateWindowW(L"Button", L"Start",
                WS_VISIBLE | WS_CHILD ,
                20, 80, 150, 25, hwnd, (HMENU) ID_BTN_START, NULL, NULL);

            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_START) {
                wchar_t buttonText[256];
                GetWindowTextW(buttonHandle, buttonText, 256);

                if (wcscmp(buttonText, L"Start") == 0) {
                    SetWindowTextW(buttonHandle, L"Stop");
                    timerID = SetTimer(hwnd, TIMER_ID, 1000, NULL);
                } else {
                    SetWindowTextW(buttonHandle, L"Start");
                    if (timerID != 0) {
                        KillTimer(hwnd, TIMER_ID);
                        timerID = 0;
                    }
                }
            }
            break;

        case WM_TIMER:
            if (wParam == TIMER_ID) {
                remainingSeconds--;
                
                int minutes = remainingSeconds / 60;
                int seconds = remainingSeconds % 60;
                
                wchar_t timeString[32];
                wsprintfW(timeString, L"%02d:%02d", minutes, seconds);

                SetWindowTextW(timerHandle, timeString);
                
                if (remainingSeconds <= 0) {
                    KillTimer(hwnd, TIMER_ID);
                    timerID = 0;
                    SetWindowTextW(buttonHandle, L"Start");
                    
                    MessageBeep(MB_ICONINFORMATION);
                    SetWindowTextW(timerHandle, L"00:00");
                }
            }
            break;

        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            HWND hwndStatic = (HWND)lParam;
            
            // 1 is static controls
            if (GetDlgCtrlID(hwndStatic) == 1)
            {
                SetTextColor(hdcStatic, RGB(50, 50, 50));
                SetBkMode(hdcStatic, TRANSPARENT);
                HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                return (LRESULT)hBrush;
            }
            break;
        }

        case WM_DESTROY:
            if (timerID != 0) {
                KillTimer(hwnd, TIMER_ID);
            }
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