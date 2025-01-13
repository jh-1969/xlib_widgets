#pragma once

typedef struct XlibWrapper_s XlibWrapper;

struct {
    int winWidth;
    int winHeight;
    int winOffLeft;
    int winOffTop;

    int closeAfterIdleForMiliseconds;

    unsigned long backgroundColor;

} typedef XlibWrapperOptions;



XlibWrapper* xlib_wrapper_init(XlibWrapperOptions* options);

void xlib_wrapper_set_font(XlibWrapper* xlib_wrapper, 
        const char* font_name);

void xlib_wrapper_draw_string(XlibWrapper* xlib_wrapper,
        char* string, int len, int x, int y);

void xlib_wrapper_draw_rectangle(XlibWrapper* xlib_wrapper,
        int x, int y, int width, int height);

void xlib_wrapper_fill_rectangle(XlibWrapper* xlib_wrapper,
        int x, int y, int width, int height);

void xlib_wrapper_clear_area(XlibWrapper* xlib_wrapper,
        int x, int y, int with, int height);

void xlib_wrapper_set_foreground(XlibWrapper* xlib_wrapper, unsigned long color);

int xlib_wrapper_loop(XlibWrapper* xlib_wrapper);

void xlib_wrapper_flush(XlibWrapper* xlib_wrapper);

void xlib_wrapper_destroy(XlibWrapper* xlib_wrapper);

//should be used right after init
void xlib_wrapper_add_callback(XlibWrapper* xlib_wrapper,
        const char* args, const char* key, void (*func)(const char*));
