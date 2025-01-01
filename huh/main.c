#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

char keys[32];

bool key_is_pressed(Display* dpy, KeySym ks) {
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    return keys_return[kc2 >> 3] & (1 << (kc2 & 7));
}

int main() {
    Display* dpy = XOpenDisplay(NULL);
    
    printf("%i, %i", XKeysymToKeycode(dpy, XK_F1),XKeysymToKeycode(dpy, XK_F1));

    while (1) {
        XQueryKeymap(dpy, keys);

        if (key_is_pressed(dpy, XK_a))
            printf("whuh\n");
        
        
    }
    XCloseDisplay(dpy);
}
