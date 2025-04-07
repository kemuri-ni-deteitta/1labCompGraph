#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static int sx, sy;
static HPEN hPenAxes, hPenSin;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = TEXT("MainFrame");
    wc.lpfnWndProc   = WndProc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName  = NULL;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if(!RegisterClass(&wc)) return 0;

    HWND hWnd = CreateWindow(
        TEXT("MainFrame"),
        TEXT("График sin(x) - одна волна"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        HWND_DESKTOP, NULL,
        hInstance, NULL
    );
    if(!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch(message)
    {
        case WM_CREATE:
            hPenAxes = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
            hPenSin  = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
            break;
        case WM_SIZE:
            sx = LOWORD(lParam);
            sy = HIWORD(lParam);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            {
                HPEN oldPen = (HPEN)SelectObject(hdc, hPenAxes);
                MoveToEx(hdc, 0, sy/2, NULL);
                LineTo(hdc, sx, sy/2);
                MoveToEx(hdc, sx/2, 0, NULL);
                LineTo(hdc, sx/2, sy);
                SelectObject(hdc, hPenSin);
                double xMin = -3.14159;
                double xMax =  3.14159;
                double dx = (xMax - xMin) / (double)sx;
                double amplitude = (sy / 2.0) * 0.8;
                bool firstPoint = true;
                for(int i = 0; i <= sx; i++)
                {
                    double x = xMin + i * dx;
                    double y = sin(x);
                    int px = i;
                    int py = (int)(sy/2 - amplitude * y);
                    if(firstPoint)
                    {
                        MoveToEx(hdc, px, py, NULL);
                        firstPoint = false;
                    }
                    else
                    {
                        LineTo(hdc, px, py);
                    }
                }
                SelectObject(hdc, oldPen);
            }
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            DeleteObject(hPenAxes);
            DeleteObject(hPenSin);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

