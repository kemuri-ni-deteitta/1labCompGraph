#include <windows.h>
#include <tchar.h>
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static HPEN hPenAxes, hPenFunc;
static int sx, sy;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = _T("MainFrame");
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
        _T("MainFrame"),
        _T("y = 5*x^2 + tan(x)"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL
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
            hPenFunc = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
            break;

        case WM_SIZE:
            sx = LOWORD(lParam);
            sy = HIWORD(lParam);
            break;

        case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);

            double xMin = -2.0;
            double xMax =  5.0;
            double yMin =  1e9;
            double yMax = -1e9;
            {
                double step = (xMax - xMin)/1000.0;
                for(double x = xMin; x <= xMax; x += step)
                {
                    double y = 5.0*x*x + tan(x);
                    if(y < yMin) yMin = y;
                    if(y > yMax) yMax = y;
                }
            }
            double margin = (yMax - yMin)*0.05;
            yMin -= margin;
            yMax += margin;

            double scaleX = (double)sx / (xMax - xMin);
            double scaleY = (double)sy / (yMax - yMin);

            auto toScreen = [&](double X, double Y)
            {
                int px = (int)((X - xMin)*scaleX);
                int py = (int)(sy - (Y - yMin)*scaleY);
                return POINT{px, py};
            };

            SelectObject(hdc, hPenAxes);

            if(yMin <= 0 && yMax >= 0)
            {
                double Y = 0.0;
                POINT p1 = toScreen(xMin, Y);
                POINT p2 = toScreen(xMax, Y);
                MoveToEx(hdc, p1.x, p1.y, NULL);
                LineTo(hdc, p2.x, p2.y);
            }

            if(xMin <= 0 && xMax >= 0)
            {
                double X = 0.0;
                POINT p1 = toScreen(X, yMin);
                POINT p2 = toScreen(X, yMax);
                MoveToEx(hdc, p1.x, p1.y, NULL);
                LineTo(hdc, p2.x, p2.y);
            }

            SelectObject(hdc, hPenFunc);
            double dx = (xMax - xMin)/ (double)sx; 
            bool firstPoint = true;
            POINT pOld;
            for(int i=0; i<=sx; i++)
            {
                double x = xMin + i*dx;
                double c = cos(x);
                if(fabs(c) < 1e-3)
                {
                    firstPoint = true;
                    continue;
                }
                double y = 5.0*x*x + tan(x);
                POINT p = toScreen(x,y);

                if(firstPoint)
                {
                    MoveToEx(hdc, p.x, p.y, NULL);
                    firstPoint=false;
                }
                else
                {
                    LineTo(hdc, p.x, p.y);
                }
                pOld = p;
            }

            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY:
            DeleteObject(hPenAxes);
            DeleteObject(hPenFunc);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

