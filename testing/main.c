#include "xlib_wrapper.h"

#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>

void test(const char* args) {
    printf("pressed: %s\n", args);
}

int main() {
    XlibWrapperWindowOptions winOptions = {
        .width = 200,
        .height = 100,
        .offLeft = 50,
        .offTop = 50
    };

    XlibWrapper* xlibWrapper = xlib_wrapper_init(&winOptions);
    
    xlib_wrapper_set_font(xlibWrapper, "a14");
    xlib_wrapper_draw_string(xlibWrapper, "hello", 5, 10, 20);

    xlib_wrapper_flush(xlibWrapper);
    
    xlib_wrapper_add_callback(xlibWrapper, "up", "F3", test);
    xlib_wrapper_add_callback(xlibWrapper, "down", "F2", test);
    
    while(xlib_wrapper_loop(xlibWrapper));

    xlib_wrapper_destroy(xlibWrapper);

    return 0;
}
