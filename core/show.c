#include "show.h"

static char* itoa(char* str, int num) {
    char* p = str;
    char ch;
    int flag = 0;

    *p++ = '0';
    *p++ = 'x';
    if(num == 0) *p++ = '0';
    else {
        for (int i = 28; i >= 0; i-=4) {
            ch = (num >> i) & 0xF;
            if(flag || (ch > 0) ) {
                flag = 1;
                ch += '0';
                if( ch > '9') ch += 7;
                *p++ = ch;
            }
        }
    }
    *p = 0;
    return str;
}

void display_str(const char* message) {
    _display_str(message);
    return;
}

void display_pose_set(int position) {
    display_pose = position;
    return;
}

void display_color_str(const char* str, int TextColor) {
    _display_color_str(str, TextColor);
    return;
}

void display_int(int input) {
    char output[16];
    itoa(output, input);
    display_str(output);
    return;
}

void display_clear() {
    display_pose_set(0);
    for (int i = 0; i < 80 * 25; i++) { display_str(" "); }
    display_pose_set(0);
}

void __stack_chk_fail() { display_str("stack_chk_fail"); }
