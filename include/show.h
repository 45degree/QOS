#ifndef SHOW_H
#define SHOW_H

#include <packaging_asm32.h>

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

extern void display_str(const char* mes);
extern void display_pose_set(int position);
extern void display_color_str(const char* str, int TextColor);
extern void display_int(int num);
extern void display_clear();

#endif
