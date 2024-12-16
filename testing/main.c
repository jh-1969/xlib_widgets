#include "xlib_wrapper.h"

#include <unistd.h>
#include <X11/Xlib.h>

int main() {
    XlibWrapperWindowOptions win_options = {
        .width = 200,
        .height = 100,
        .off_left = 50,
        .off_top = 50
    };

    XlibWrapper* xlib_wrapper = xlib_wrapper_init(&win_options);

    sleep(1);

    xlib_wrapper_destroy(xlib_wrapper);

    return 0;
}
