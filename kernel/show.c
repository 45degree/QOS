#include "show.h"

static void itoa_hex(char* str, int num) {
    int flags = 0;
    *str++ = '0';
    *str++ = 'x';

    if(num == 0) {
        *str++ = '0';
        *str = 0;
        return;
    }

    for(int i = 28; i >= 0; i-=4) {
        char ch = (num >> i) & 0xF;
        if(flags || ch > 0) {
            flags = 1;
            ch += '0';
            if(ch > '9') ch = ch - '9' + 'A' - 1;
            *str++ = ch;
        }
    }
    *str = 0;
}

int itoa(char* str, int num, unsigned int radix) {
    if(radix == 16) {
        itoa_hex(str, num);
        return 0;
    }

    if(radix == 10 && num < 0){
        *str++ = '-';
        num = -num;
    }

    int r = 0;
    char* p = str;
    do {
        r = num % radix;
        num = num / radix;
        *p++ = r + '0';
    }while(num > 0);

    *p-- = 0;
    while(p > str) {
        char temp = *p;
        *p = *str;
        *str = temp;
        p--;
        str++;
    }
    return 0;
}

void display_str(const char* message) { _display_str(message); }

void display_color_str(const char* str, int TextColor) { _display_color_str(str, TextColor); }

void display_int(int input) {
    char output[16];
    itoa(output, input, 16);
    display_str(output);
}

void display_clear() {
    set_display_pose(0);
    for (int i = 0; i < 80 * 25; i++) { display_str(" "); }
    set_display_pose(0);
}

void __stack_chk_fail() { display_str("stack_chk_fail"); }
