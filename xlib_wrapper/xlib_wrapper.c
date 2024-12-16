#include "xlib_wrapper.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>



static Window
static_create_window(XlibWrapperWindowOptions* window_options, XlibWrapper* xlib_wrapper);



struct XlibWrapper_s {
    Display* d;
    Window root;
    Window win;
    GC gc;
    XFontStruct* font_info;
};



XlibWrapper* xlib_wrapper_init(XlibWrapperWindowOptions* window_options) {
    XlibWrapper* xlib_wrapper;

    xlib_wrapper->d = XOpenDisplay(NULL);
    xlib_wrapper->root = DefaultRootWindow(xlib_wrapper->d);
    xlib_wrapper->win = static_create_window(window_options, xlib_wrapper); 

    return xlib_wrapper;
}

void xlib_wrapper_destroy(XlibWrapper* xlib_wrapper) {
    XUnmapWindow(xlib_wrapper->d, xlib_wrapper->win);
    XCloseDisplay(xlib_wrapper->d);
}



static Window
static_create_window(XlibWrapperWindowOptions* window_options, XlibWrapper* xlib_wrapper) {
    XSetWindowAttributes attrs;
    attrs.override_redirect = true;

    XVisualInfo vinfo;
    if (!XMatchVisualInfo(xlib_wrapper->d, DefaultScreen(xlib_wrapper->d), 32, TrueColor, &vinfo)) {
        printf("No visual found supporting 32 bit color, terminating\n");
        exit(EXIT_FAILURE);
    }

    attrs.colormap = XCreateColormap(xlib_wrapper->d, xlib_wrapper->root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    Window win = XCreateWindow(
        xlib_wrapper->d, xlib_wrapper->root,
        window_options->off_left, window_options->off_top, window_options->width, window_options->height, 0,
        vinfo.depth, InputOutput, 
        vinfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, &attrs
    );
    return win;
}
