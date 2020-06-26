//
// Created by qijinwen on 5/5/20.
//

#ifndef QOS_CONSOLE_H
#define QOS_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

#define V_MEM_BASE 0xB8000u //!< 显存基地址
#define V_MEM_SIZE 0x8000u  //!< 32K的显存大小

#define CRTC_ADDR_REG 0x3D4 //!< CRT 控制寄存器的中的地址寄存器
#define CRTC_DATA_REG 0x3D5 //!< CRT 控制寄存器的中的数据寄存器
#define START_ADDR_H 0xC    //!< 显存地址开始位置寄存器(高地址)
#define START_ADDR_L 0xD    //!< 显存地址开始位置寄存器(低地址)
#define CURSOR_H 0xE        //!< 光标位置寄存器(高地址)
#define CURSOR_L 0xF        //!< 光标位置寄存器(低地址)

#define SCR_UP 1  //!< scroll forward
#define SCR_DN -1 //!< scroll backward

#define SCREEN_SIZE (80 * 25) //!< 一个console的大小
#define SCREEN_WIDTH 80       //!< 一个console的宽度

typedef struct console {
    u32 current_start_addr; //!< 当前控制台相对于显存(0B8000H)的启始地址, 单位(字)
    u32 original_addr;      //!< 当前控制台相对于显存(0B8000H)的开始地址, 单位(字)
    u32 video_mem_limit;    //!< 当前控制台相对于显存(0B8000H)的最大显存, 单位(字)
    u32 cursor; //!< 当前控制台的光标位置，记载的是当前显示字符的位置到显存(0B8000H)的开始地址的距离,单位(字)
} CONSOLE;

/**
 * @brief 初始化控制台
 * @param console           需要被初始化的控制台
 * @param original_addr     控制台在显存中的开始地址
 * @param video_mem_limit   控制台使用的显存大小
 * @note original_addr的开始地址是从0开始的, 单位是字而不是字节
 * @author 45degree
 * @since 0.0.1
 */
void init_console(CONSOLE* console, u32 original_addr, u32 video_mem_limit);

/**
 * @brief 向当前的控制台输出一个字符
 * @param console
 * @param str
 */
void console_output_char(CONSOLE* console, char str);

/**
 * @brief 在屏幕中显示控制台
 * @param console
 */
void show_console(CONSOLE* console);

void scroll_screen(CONSOLE* console, int direction);

#ifdef __cplusplus
};
#endif

#endif // QOS_CONSOLE_H
