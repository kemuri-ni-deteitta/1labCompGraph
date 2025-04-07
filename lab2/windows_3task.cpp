#include <windows.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static HPEN hpen, hpen1;
static int sx, sy;
int a, b, n, x_scr, y_scr, z_scr;
double x, y, z;
double x_min, x_max, y_min, y_max;
double Kx, Ky;

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

    if (!RegisterClass(&wc)) return 0;

    HWND hWnd = CreateWindow(
        TEXT("MainFrame"),
        TEXT("Поверхность"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        HWND_DESKTOP, NULL,
        hInstance, NULL
    );
    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
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

    switch (message)
    {
        case WM_CREATE:
            hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
            break;

        case WM_SIZE:
            sx = LOWORD(lParam);
            sy = HIWORD(lParam);
            a = sx / 2;
            b = sy / 2;
            break;

        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);

            x_min = -100;
            x_max =  100;
            y_min = -100;
            y_max =  100;

            Kx = (sx - 220) / (x_max - x_min);
            Ky = (sy - 220) / (y_max - y_min);

            SelectObject(hdc, hpen);

            MoveToEx(hdc, 0, b, NULL);
            LineTo(hdc, sx, b);

            MoveToEx(hdc, a, 0, NULL);
            LineTo(hdc, a, sy);

            MoveToEx(hdc, 20, sy - 20, NULL);
            LineTo(hdc, sx - 20, 20);

            n = y_max - y_min;

            for (y = y_min; y < y_max; y += 5)
            {
                hpen1 = CreatePen(PS_SOLID, 2,
                    RGB(255,
                        255 - (int)(255.0 * n * (y + y_min) / (n * n)),
                        255 - (int)(255.0 * n * (y + y_min) / (n * n)))
                );
                SelectObject(hdc, hpen1);

                z = b - 100 * sin(M_PI / 50 * sqrt(x_min * x_min + y * y));
                MoveToEx(hdc,
                         (int)(x_min * Kx + a + y),
                         (int)(z + y),
                         NULL);

                for (x = x_min; x < x_max; x += 5)
                {
                    z = b - 100 * sin(M_PI / 50 * sqrt(x * x + y * y));
                    LineTo(hdc,
                           (int)(x * Kx + a + y),
                           (int)(z + y));
                }
                DeleteObject(hpen1);
            }
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            DeleteObject(hpen);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

