#include "xlib_wrapper.h"

#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>



static Window
static_create_window(XlibWrapper* xlib_wrapper, XlibWrapperWindowOptions* window_options);

static GC
static_create_gc(XlibWrapper* xlib_wrapper);

static void
static_handle_event(XlibWrapper* xlib_wrapper, XEvent ev);



struct {
    const char* args;
    KeyCode keycode;
    void (*func)(const char*);
} typedef Callback;



struct XlibWrapper_s {
    Display* d;
    Window root;
    Window win;
    Window focused;
    GC gc;
    Font font;

    Callback* callbacks;
    uint16_t callbacksCount;

    struct pollfd fds;
};



XlibWrapper*
xlib_wrapper_init(XlibWrapperWindowOptions* window_options) {
    XlibWrapper* xlib_wrapper;

    xlib_wrapper->d = XOpenDisplay(NULL);
    xlib_wrapper->root = DefaultRootWindow(xlib_wrapper->d);

    xlib_wrapper->win = static_create_window(xlib_wrapper, window_options);

    int rev = 0;
    XGetInputFocus(xlib_wrapper->d, &xlib_wrapper->focused, &rev);
    
    xlib_wrapper->gc = static_create_gc(xlib_wrapper);

    XMapWindow(xlib_wrapper->d, xlib_wrapper->win);
    
    xlib_wrapper->font = -1;

    xlib_wrapper->callbacks = NULL;
    xlib_wrapper->callbacksCount = 0;
    
    XSelectInput(xlib_wrapper->d, xlib_wrapper->focused, KeyPressMask|FocusChangeMask);

    xlib_wrapper->fds.fd = XConnectionNumber(xlib_wrapper->d);
    xlib_wrapper->fds.events = POLLIN;

    return xlib_wrapper;
}

void
xlib_wrapper_set_font(XlibWrapper* xlib_wrapper, const char* font_name) {
    xlib_wrapper->font = XLoadFont(xlib_wrapper->d, font_name);
    //i dont know how to check for bad name with XLoadFont
    //XQueryFont and XLoadQueryFont causes a seg fault ???
    //
    //  !! asi neco debilniho musim to nekdy vyresit !!
    XSetFont(xlib_wrapper->d, xlib_wrapper->gc, xlib_wrapper->font);
}

void
xlib_wrapper_draw_string(XlibWrapper* xlib_wrapper,
                         const char* string, int len, int x, int y) {
    XDrawString(xlib_wrapper->d, xlib_wrapper->win, xlib_wrapper->gc, x, y, string, len);
}

int
xlib_wrapper_loop(XlibWrapper* xlib_wrapper) {
    int poll_ret = poll(&xlib_wrapper->fds, 1, 1500);
    
    if(poll_ret <= 0)
        return 0;
    else {
        while(XPending(xlib_wrapper->d) > 0) {
            XEvent ev;
            XNextEvent(xlib_wrapper->d, &ev);
            static_handle_event(xlib_wrapper, ev);
        }

        XFlush(xlib_wrapper->d);
        return 1;
    }
}

void 
xlib_wrapper_flush(XlibWrapper* xlib_wrapper) {
    XFlush(xlib_wrapper->d);
}

void
xlib_wrapper_destroy(XlibWrapper* xlib_wrapper) {
    if(xlib_wrapper->callbacks != NULL)
        free(xlib_wrapper->callbacks);
    
    if(xlib_wrapper->font != -1)
        XUnloadFont(xlib_wrapper->d, xlib_wrapper->font);

    XUnmapWindow(xlib_wrapper->d, xlib_wrapper->win);
    XCloseDisplay(xlib_wrapper->d);
}

void
xlib_wrapper_add_callback(XlibWrapper* xlib_wrapper,
                          const char* args, const char* key, void (*func)(const char*)) {
    xlib_wrapper->callbacksCount++;
    xlib_wrapper->callbacks = realloc(xlib_wrapper->callbacks,
                                      sizeof(Callback) * xlib_wrapper->callbacksCount);

    if(xlib_wrapper->callbacks == NULL) {
        printf("realloc failed\n");
        exit(1);
    }
    
    Callback callback = {
        args,
        XKeysymToKeycode(xlib_wrapper->d, XStringToKeysym(key)),
        func
    };

    xlib_wrapper->callbacks[xlib_wrapper->callbacksCount - 1] = callback;
}



static Window
static_create_window(XlibWrapper* xlib_wrapper, XlibWrapperWindowOptions* window_options) {
    XSetWindowAttributes attrs;
    attrs.override_redirect = true;

    XVisualInfo vinfo;
    if(!XMatchVisualInfo(xlib_wrapper->d, DefaultScreen(xlib_wrapper->d), 32, TrueColor, &vinfo)) {
        printf("No visual found supporting 32 bit color\n");
        exit(1);
    }

    attrs.colormap = XCreateColormap(xlib_wrapper->d, xlib_wrapper->root, vinfo.visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;

    Window win = XCreateWindow(
        xlib_wrapper->d, xlib_wrapper->root,
        window_options->offLeft, window_options->offTop,
        window_options->width, window_options->height,
        0, vinfo.depth, InputOutput, vinfo.visual,
        CWOverrideRedirect | CWColormap | CWBackPixel | CWBorderPixel, &attrs
    );
    return win;
}

static GC
static_create_gc(XlibWrapper* xlib_wrapper) { 
    GC gc;
    unsigned long valuemask = 0;
    
    XGCValues values;
    unsigned int line_width = 2;
    int line_style = LineSolid;
    int cap_style = CapButt;
    int join_style = JoinBevel;
    int screen_num = DefaultScreen(xlib_wrapper->d);
    gc = XCreateGC(xlib_wrapper->d, xlib_wrapper->win, valuemask, &values);
    
    if(gc < 0) {
        printf("Failed to create GC\n");
        exit(1);
    }
    
    XSetForeground(xlib_wrapper->d, gc, WhitePixel(xlib_wrapper->d, screen_num));
    XSetBackground(xlib_wrapper->d, gc, BlackPixel(xlib_wrapper->d, screen_num));

    XSetLineAttributes(xlib_wrapper->d, gc, line_width, line_style, cap_style, join_style);
    XSetFillStyle(xlib_wrapper->d, gc, FillSolid);
    return gc;
}

static void
static_handle_event(XlibWrapper* xlib_wrapper, XEvent ev) {
    switch(ev.type) {
        case FocusOut:
            if (xlib_wrapper->focused != xlib_wrapper->root)
                XSelectInput(xlib_wrapper->d, xlib_wrapper->focused, 0);

            int rev = 0;
            XGetInputFocus(xlib_wrapper->d, &xlib_wrapper->focused, &rev);

            if(xlib_wrapper->focused == PointerRoot)
                xlib_wrapper->focused = xlib_wrapper->root;
            XSelectInput(xlib_wrapper->d, xlib_wrapper->focused, KeyPressMask|FocusChangeMask);
        break;

        case KeyPress:
            for(int i = 0; i < xlib_wrapper->callbacksCount; i++) {
                if(ev.xkey.keycode == xlib_wrapper->callbacks[i].keycode) {
                    xlib_wrapper->callbacks[i].func(xlib_wrapper->callbacks[i].args);
                }
            }
        break;
    }
}
