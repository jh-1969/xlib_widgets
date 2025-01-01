#pragma once

typedef struct XlibWrapper_s XlibWrapper;

struct {
    int width;
    int height;
    int offLeft;
    int offTop;
} typedef XlibWrapperWindowOptions;



XlibWrapper* xlib_wrapper_init(XlibWrapperWindowOptions* window_options);

void xlib_wrapper_set_font(XlibWrapper* xlib_wrapper, 
        const char* font_name);

void xlib_wrapper_draw_string(XlibWrapper* xlib_wrapper,
        const char* string, int len, int x, int y);

int xlib_wrapper_loop(XlibWrapper* xlib_wrapper);

void xlib_wrapper_flush(XlibWrapper* xlib_wrapper);

void xlib_wrapper_destroy(XlibWrapper* xlib_wrapper);

void xlib_wrapper_add_callback(XlibWrapper* xlib_wrapper,
        const char* args, const char* key, void (*func)(const char*));
