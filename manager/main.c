#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    system("pkill sxhkd; sxhkd -c /home/jh/.config/sxhkd/sxhkdrc_widgetless &");
    
    system("/home/jh/Documents/cdev/xlib_widgets_folder/xlib_widgets/bright/Xbright d");

    system("pkill sxhkd; sxhkd &");
}
