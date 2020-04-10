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

void disp_str(const char* message) {
    _disp_str(message);
    return;
}

void disp_pose_set(int position) {
    int maxPose = 80 * 25;      // 每行80个字符，共25行
    if (position > maxPose || position < 0) position = 0;
    int realPose = position * 2;
    _disp_pose_set(realPose);
    return;
}

void disp_color_str(const char* str, int TextColor) {
    _disp_color_str(str, TextColor);
    return;
}

void disp_int(int input) {
    char output[16];
    itoa(output, input);
    disp_str(output);
    return;
}

void __stack_chk_fail() {
    disp_str("error");
}