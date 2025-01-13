#include "xlib_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define WIDTH 130
#define HEIGHT 32

XlibWrapper* xlibWrapper;

int percentage = 0;

char* percentage_to_string(int num, int* len) {
    int length = snprintf(NULL, 0, "%d", num);
    char* str = malloc(length + 2);
    snprintf(str, length + 2, "%d", num);
    str[length] = '%';
    str[length + 1] = '\0';

    *len = length + 1;
    return str;
}

void change(const char* args) {
    if(strcmp(args, "u") == 0)
        percentage += 5;
    else
        percentage -= 5;

    if(percentage > 100)
        percentage = 100;
    else if(percentage < 0)
        percentage = 0;

    xlib_wrapper_clear_area(xlibWrapper, 89, 5, 35, 24);
    
    int len = 0;
    char* str = percentage_to_string(percentage, &len);
    xlib_wrapper_draw_string(xlibWrapper, str, len, 89, 20);
    
    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;

        case 0:
            {}
            char* argv[] = {"brightnessctl", "set", str, "-q", (char*)0};
            char* env[] = {(char*)0};
            execve("/bin/brightnessctl", argv, env);
            perror("execve");
            break;
    }

    free(str);
}

int main() {
    FILE* fp;
    char path[4];

    fp = popen("brightnessctl -m | sed 's/.*,\\(.*\\)%.*/\\1/'", "r");
    
    if (fp == NULL) {
        printf("Failed to run command" );
        exit(1);
    }
    
    fscanf(fp, "%d", &percentage); 
    pclose(fp);
    
    XlibWrapperOptions options = {
        .winWidth = WIDTH,
        .winHeight = HEIGHT,
        .winOffLeft = 20,
        .winOffTop = 20,

        .closeAfterIdleForMiliseconds = 1500,

        .backgroundColor = 0x232323 
    };
    
    xlibWrapper = xlib_wrapper_init(&options);
    
    xlib_wrapper_add_callback(xlibWrapper, "u", "F6", change);
    xlib_wrapper_add_callback(xlibWrapper, "d", "F5", change);
    
    xlib_wrapper_set_font(xlibWrapper, "a14");

    xlib_wrapper_set_foreground(xlibWrapper, 0xaeadaf);
    xlib_wrapper_draw_string(xlibWrapper, "brightness", 10, 10, 20);
    
    int len = 0;
    char* str = percentage_to_string(percentage, &len);
    xlib_wrapper_draw_string(xlibWrapper, str, len, 89, 20);
    free(str);

    xlib_wrapper_draw_rectangle(xlibWrapper, 0, 0, WIDTH, HEIGHT);
    xlib_wrapper_flush(xlibWrapper);

    while(xlib_wrapper_loop(xlibWrapper));

    xlib_wrapper_destroy(xlibWrapper);
    
    return 0;
}
