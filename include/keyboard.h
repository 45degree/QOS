#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "global.h"

#define KB_IN_BYTES 32 /* size of keyboard input buffer */

/**
 * @brief 键盘输入缓冲区
 */
typedef struct keyboard_input_buffer {
    char *head;            //!< 指向缓冲区中下一个空闲位置
    char *tail;            //!< 指向键盘任务处理的字节
    int count;             //!< 缓冲区字节总数
    char buf[KB_IN_BYTES]; //!< 缓冲区
} KB_INPUT_BUFFER;

/**
 * @brief 键盘中断处理函数
 */
void keyboard_handler(int irq);

/**
 * @brief 初始化键盘中断
 */
void init_keyboard();

/**
 * @brief
 */
void keyboard_read();

/**
 *
 * @param key
 */
void in_process(u32 key);

#endif