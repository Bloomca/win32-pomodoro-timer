#include <windows.h>
#include <dwmapi.h>

#define ID_BTN_START 1001
#define ID_BTN_RESET 1002
#define ID_BTN_ADD_MINUTE 1003
#define ID_BTN_REMOVE_MINUTE 1004
#define TIMER_ID 1

static UINT_PTR timerID = 0;
static int remainingSeconds = 25 * 60;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void updateTimer();

HWND timerHandle;
HWND startButtonHandle;
HWND resetButtonHandle;
HWND addMinuteButtonHandle;
HWND removeMinuteButtonHandle;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    MSG msg;
    WNDCLASSW wc = {0};

    wc.lpszClassName = L"Timer App";
    wc.hInstance = hInstance;
    wc.hbrBackground = NULL;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClass(&wc);
    CreateWindow(wc.lpszClassName, L"Timer app", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 375, 350, 0, 0, hInstance, 0);

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
            int backdropType = 2; // DWMSBT_MAINWINDOW (Mica)
            DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

            timerHandle = CreateWindowW(L"Static", L"25:00", WS_CHILD | WS_VISIBLE | SS_LEFT,
                125, 20, 120, 60, 
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

            startButtonHandle = CreateWindowW(L"Button", L"Start",
                WS_VISIBLE | WS_CHILD ,
                20, 80, 150, 25, hwnd, (HMENU) ID_BTN_START, NULL, NULL);

            resetButtonHandle = CreateWindowW(L"Button", L"Reset",
                WS_VISIBLE | WS_CHILD ,
                190, 80, 150, 25, hwnd, (HMENU) ID_BTN_RESET, NULL, NULL);

            addMinuteButtonHandle = CreateWindowW(L"Button", L"+",
                WS_VISIBLE | WS_CHILD ,
                250, 30, 50, 25, hwnd, (HMENU) ID_BTN_ADD_MINUTE, NULL, NULL);

            removeMinuteButtonHandle = CreateWindowW(L"Button", L"-",
                WS_VISIBLE | WS_CHILD ,
                50, 30, 50, 25, hwnd, (HMENU) ID_BTN_REMOVE_MINUTE, NULL, NULL);

            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_START) {
                wchar_t buttonText[256];
                GetWindowTextW(startButtonHandle, buttonText, 256);

                if (wcscmp(buttonText, L"Start") == 0) {
                    if (remainingSeconds == 0) {
                        remainingSeconds = 25 * 60;
                        updateTimer();
                    }

                    SetWindowTextW(startButtonHandle, L"Stop");
                    timerID = SetTimer(hwnd, TIMER_ID, 1000, NULL);
                } else {
                    SetWindowTextW(startButtonHandle, L"Start");
                    if (timerID != 0) {
                        KillTimer(hwnd, TIMER_ID);
                        timerID = 0;
                    }
                }
            } else if (LOWORD(wParam) == ID_BTN_RESET) {
                SetWindowTextW(startButtonHandle, L"Start");
                if (timerID != 0) {
                    KillTimer(hwnd, TIMER_ID);
                    timerID = 0;
                }

                remainingSeconds = 25 * 60;

                updateTimer();
            } else if (LOWORD(wParam) == ID_BTN_REMOVE_MINUTE) {
                if (remainingSeconds >= 60) {
                    remainingSeconds -= 60;
                } else {
                    remainingSeconds = 0;
                }
                
                updateTimer();
            } else if (LOWORD(wParam) == ID_BTN_ADD_MINUTE) {
                remainingSeconds += 60;
                updateTimer();
            }
            break;

        case WM_TIMER:
            if (wParam == TIMER_ID) {
                if (remainingSeconds > 0) {
                    remainingSeconds--;
                }
                
                updateTimer();
                
                if (remainingSeconds <= 0) {
                    KillTimer(hwnd, TIMER_ID);
                    timerID = 0;
                    SetWindowTextW(startButtonHandle, L"Start");
                    
                    MessageBeep(MB_ICONINFORMATION);
                    SetWindowTextW(timerHandle, L"00:00");
                }
            }
            break;

        // needed for transparent background for Timer text
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

void updateTimer() {
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    
    wchar_t timeString[32];
    wsprintfW(timeString, L"%02d:%02d", minutes, seconds);

    SetWindowTextW(timerHandle, timeString);
}