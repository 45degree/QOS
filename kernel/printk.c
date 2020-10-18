#include "printk.h"
#include "core_string.h"
#include "package_iA32/packaging_iA32.h"
#include "show.h"
#include "type.h"

int vsprintk(char* buf, const char* fmt, va_list args) {
    char temp[256];
    char* p = buf;
    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            *p++ = *fmt;
            continue;
        }
        fmt++;
        switch (*fmt) {
        case 'x':
            itoa(temp, va_arg(args, int), 16);
            core_strcpy(p, temp);
            p += core_strlen(temp);
            break;
        case 'd':
            itoa(temp, va_arg(args, int), 10);
            core_strcpy(p, temp);
            p += core_strlen(temp);
            break;
        case 'c':
            *p++ = va_arg(args, int);
            break;
        case 's':
            core_strcpy(p, va_arg(args, char*));
            int len = core_strlen(p);
            p += len;
            break;
        default:
            break;
        }
    }
    return (p - buf);
}

void printk(const char* fmt, ...) {
    char buf[256];
    core_memset(buf, 0, 256);
    va_list list;
    va_start(list, fmt);
    vsprintk(buf, fmt, list);
    printx(buf);
}
