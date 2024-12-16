#pragma once

typedef struct XlibWrapper_s XlibWrapper;

struct {
    int width;
    int height;
    int off_left;
    int off_top;
} typedef XlibWrapperWindowOptions;



XlibWrapper* xlib_wrapper_init(XlibWrapperWindowOptions* window_options);
void xlib_wrapper_destroy(XlibWrapper* xlib_wrapper);
