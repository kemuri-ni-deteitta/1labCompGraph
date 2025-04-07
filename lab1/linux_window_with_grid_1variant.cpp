#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cstdlib>
#include <iostream>
#include <cmath>

void DrawEquilateralTriangle(Display* display, Window window, GC gc,
                             int cx, int cy, int sideLength, unsigned long color)
{
    double height = (std::sqrt(3.0) / 2.0) * sideLength;

    int x1 = cx;
    int y1 = cy - static_cast<int>(height / 2);
    int x2 = cx - sideLength / 2;
    int y2 = cy + static_cast<int>(height / 2);
    int x3 = cx + sideLength / 2;
    int y3 = cy + static_cast<int>(height / 2);

    XSetForeground(display, gc, color);
    XSetLineAttributes(display, gc, 4, LineSolid, CapButt, JoinMiter);

    XDrawLine(display, window, gc, x1, y1, x2, y2);
    XDrawLine(display, window, gc, x2, y2, x3, y3);
    XDrawLine(display, window, gc, x3, y3, x1, y1);
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        std::cerr << "Не удалось открыть дисплей X" << std::endl;
        return 1;
    }

    int screen = DefaultScreen(display);
    int winWidth = 800;
    int winHeight = 600;
    Window window = XCreateSimpleWindow(display,
                                        RootWindow(display, screen),
                                        10, 10,
                                        winWidth, winHeight,
                                        1,
                                        BlackPixel(display, screen),
                                        WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDelete, 1);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, nullptr);
    XSetForeground(display, gc, BlackPixel(display, screen));

    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        switch(event.type) {
            case Expose: {
                for (int x = 0; x < winWidth; x += (winWidth == 0 ? 1 : winWidth / 10)) {
                    XDrawLine(display, window, gc, x, 0, x, winHeight);
                }
                for (int y = 0; y < winHeight; y += (winHeight == 0 ? 1 : winHeight / 10)) {
                    XDrawLine(display, window, gc, 0, y, winWidth, y);
                }

                unsigned long red       = 0xFF0000;
                unsigned long green     = 0x00FF00;
                unsigned long blue      = 0x0000FF;
                unsigned long lightBlue = 0x1CF1FF;
                unsigned long magenta   = 0xFF00FF;

                DrawEquilateralTriangle(display, window, gc, 200, 200, 100, red);
                DrawEquilateralTriangle(display, window, gc, 250, 220, 100, green);
                DrawEquilateralTriangle(display, window, gc, 300, 200, 100, blue);
                DrawEquilateralTriangle(display, window, gc, 275, 250, 100, lightBlue);
                DrawEquilateralTriangle(display, window, gc, 225, 250, 100, magenta);
                break;
            }
            case ConfigureNotify:
                winWidth = event.xconfigure.width;
                winHeight = event.xconfigure.height;
                break;
            case ClientMessage:
                if ((Atom) event.xclient.data.l[0] == wmDelete)
                    running = false;
                break;
            case KeyPress:
                running = false;
                break;
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
