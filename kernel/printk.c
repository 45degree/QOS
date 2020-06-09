#include "printk.h"
#include "package_iA32/packaging_iA32.h"
#include "show.h"
#include "core_string.h"

int vsprintk(char* buf, const char* fmt, char* args) {
    char temp[256];
    char* p = buf;
    char* next_arg = args;
    char** str;
    for(; *fmt; fmt++) {
        if(*fmt != '%') {
            *p++ = *fmt;
            continue;
        }
        fmt++;
        switch (*fmt) {
        case 'x':
            itoa(temp, *((int*)next_arg));
            core_strcpy(p, temp);
            next_arg += sizeof(char*);
            p += core_strlen(temp);
            break;
        case 'c':
            *p++ = *next_arg;
            next_arg += sizeof(char*);
            break;
        case 's':
            str = next_arg;
            core_strcpy(p, *str);
            int len = core_strlen(p);
            next_arg += sizeof(char*);
            p += len;
            break;
        default:
            break;
        }
    }
    return (p - buf);
}

int printk(const char* fmt, ...) {
    int i;
    char buf[256];
    char* arg = (&fmt) + 1;
    i = vsprintk(buf, fmt, arg);
    printx(buf);
}