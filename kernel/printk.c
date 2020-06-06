#include "printk.h"
#include "package_iA32/packaging_iA32.h"
#include "show.h"
#include "core_string.h"

typedef char* va_list;
#define __vasz(x) ((sizeof(x) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(ap, parmN) ((ap) = (va_list)&parmN + __vasz(parmN))
#define va_arg(ap, type) \
    (*((type*) ((va_list)((ap) = (void*)((va_list)(ap) + __vasz(type))) - __vasz(type))))
#define va_end(ap)

/**
 * @brief 格式化字符串
 * @param[out] buf 输出的格式化后字符串
 * @param[in]  fmt 格式化字符串
 * @param[in]  args 格式化的输出字符
 * @return 最后一个格式化字符的结尾位置
 * @author 45degree
 * @since 0.01
 * @note 暂时只支持"%x"的格式化
 */
static int vsprintk(char* buf, const char* fmt, char* args) {
    char temp[256];
    char* p;
    char* next_arg = args;
    for(p = buf; *fmt; fmt++) {
        if(*fmt != '%') {
            *p++ = *fmt;
            continue;
        }
        fmt++;
        switch (*fmt) {
        case 'x':
            itoa(temp, *((int*)next_arg));
            core_strcpy(p, temp);
            next_arg += 4;
            p += core_strlen(temp);
            break;
        case 'c':
            *p++ = *next_arg;
            next_arg += 4;
            break;
        case 's':
            core_strcpy(p, next_arg);
            int len = core_strlen(next_arg);
            next_arg += len;
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
    char buf2[256];
    char* arg = (&fmt) + sizeof(fmt);
    int ii = sizeof(fmt);
    int t = *((int*)arg);
    if(t == 12) {
        display_str("1");
    }
    else {
        display_str("2");
    }
    
    itoa(buf, t);
    itoa(buf2, arg);
    display_str(buf);
    display_str("\n");
    display_str(buf2);
    while(1);
    i = vsprintk(buf, fmt, arg);
    write(buf, i);
}