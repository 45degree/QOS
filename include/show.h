#ifndef QOS_SHOW_H
#define QOS_SHOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "package_iA32/packaging_iA32.h"

/*
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK 0x0                /* 0000 */
#define WHITE 0x7                /* 0111 */
#define RED 0x4                  /* 0100 */
#define GREEN 0x2                /* 0010 */
#define BLUE 0x1                 /* 0001 */
#define FLASH 0x80               /* 1000 0000 */
#define BRIGHT 0x08              /* 0000 1000 */
#define MAKE_COLOR(x, y) (x | y) /* MAKE_COLOR(Background,Foreground) */

#define DEFAULT_CHAR_COLOR (MAKE_COLOR(BLACK, WHITE))
#define GRAY_CHAR (MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define RED_CHAR (MAKE_COLOR(BLUE, RED) | BRIGHT)

extern void display_str(const char* message);
extern void display_color_str(const char* str, int TextColor);
extern void display_int(int num);
extern void display_clear();


/**
 * @brief 将数字按照给定的进位制(8, 10, 16)转化成字符串
 *
 * @param str 字符串起始位置
 * @param num 需要转换的字符串
 * @param radix 转换的进制
 *
 * @return 1 成功; 0 radix输入错误
 */
int itoa(char* str, int num, unsigned int radix);

#ifdef __cplusplus
};
#endif

#endif
