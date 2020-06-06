#include "printk.h"
#include "package_iA32/packaging_iA32.h"
#include "show.h"
#include "core_string.h"

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
            next_arg += 1;
            p += core_strlen(temp);
            break;
        case 'c':
            *p++ = *next_arg;
            next_arg += 1;
            break;
        case 's':
            str = next_arg;
            core_strcpy(p, *str);
            int len = core_strlen(p);
            next_arg += len;
            p += len;
            break;
        default:
            break;
        }
    }
    return (p - buf);
}

int printk(const char* fmt, char* arc) {
    int i;
    char buf[256];
    char* arg = (&fmt) + 1;
    i = vsprintk(buf, fmt, arg);
    write(buf, i);
}