#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cstdlib>
#include <iostream>

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        std::cerr << "Не удалось открыть дисплей X" << std::endl;
        return 1;
    }

    int screen = DefaultScreen(display);

    int width = 640;
    int height = 480;
    Window root = RootWindow(display, screen);
    Window window = XCreateSimpleWindow(
        display,
        root,
        10, 10,
        width, height,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);

    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDelete, 1);

    XMapWindow(display, window);

    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == ClientMessage) {
            if ((Atom) event.xclient.data.l[0] == wmDelete) {
                running = false;
            }
        }

        if (event.type == KeyPress) {
            running = false;
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
