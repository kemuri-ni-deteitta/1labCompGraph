#include <windows.h>
#include <tchar.h>
#include <cmath>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");

void DrawEquilateralTriangle(HDC hdc, int cx, int cy, int sideLength, COLORREF color)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);

    double height = (std::sqrt(3.0) / 2.0) * sideLength;

    int x1 = cx;
    int y1 = cy - static_cast<int>(height / 2);
    int x2 = cx - sideLength / 2;
    int y2 = cy + static_cast<int>(height / 2);
    int x3 = cx + sideLength / 2;
    int y3 = cy + static_cast<int>(height / 2);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    LineTo(hdc, x3, y3);
    LineTo(hdc, x1, y1);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

int APIENTRY WinMain(HINSTANCE This, HINSTANCE Prev, LPSTR cmd, int mode)
{
    WNDCLASS wc;
    wc.hInstance     = This;
    wc.lpszClassName = WinName;
    wc.lpfnWndProc   = WndProc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName  = NULL;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) return 0;

    HWND hWnd = CreateWindow(
        WinName,
        _T("MainFrame"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        NULL,
        This,
        NULL
    );

    if (!hWnd) return 0;

    ShowWindow(hWnd, mode);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int sx = 0;
    static int sy = 0;

    switch (message)
    {
        case WM_SIZE:
            sx = LOWORD(lParam);
            sy = HIWORD(lParam);
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            for (int x = 0; x < sx; x += (sx == 0 ? 1 : sx / 10)) {
                MoveToEx(hdc, x, 0, NULL);
                LineTo(hdc, x, sy);
            }
            for (int y = 0; y < sy; y += (sy == 0 ? 1 : sy / 10)) {
                MoveToEx(hdc, 0, y, NULL);
                LineTo(hdc, sx, y);
            }

            DrawEquilateralTriangle(hdc, 200, 200, 100, RGB(255, 0, 0));
            DrawEquilateralTriangle(hdc, 250, 220, 100, RGB(0, 255, 0));
            DrawEquilateralTriangle(hdc, 300, 200, 100, RGB(0, 0, 255));
            DrawEquilateralTriangle(hdc, 275, 250, 100, RGB(255, 255, 0));
            DrawEquilateralTriangle(hdc, 225, 250, 100, RGB(255, 0, 255));

            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
