#ifndef QOS_KEYBOARD_H
#define QOS_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"
#include "tty/tty.h"

#define KB_IN_BYTES 32 //!< 键盘输入缓冲区的大小
#define KB_CMD 0x64    //!< 键盘输入输出缓冲区端口
#define KB_DATE 0x60   //!< 键盘控制寄存器、状态寄存器端口
#define LED_CODE 0xED  //!< 设置键盘的LED
#define KB_ACK 0xFA    //!< 设置完LED后键盘返回的ACK

/**
 * @brief 键盘输入缓冲区, 键盘的输入会保存在改缓冲区
 * @note  该缓冲区保存的是键盘中断产生的扫描码
 * @author 45degree
 * @since 0.0.1
 */
typedef struct keyboard_input_buffer {
    char *head;            //!< 指向缓冲区中下一个空闲位置
    char *tail;            //!< 指向键盘任务处理的字节
    int count;             //!< 缓冲区字节总数
    char buf[KB_IN_BYTES]; //!< 缓冲区
} KB_INPUT_BUFFER;

/**
 * @brief 键盘字符处理函数集合
 * @note 函数参数中的key是解析过后的字符, 在keymap.h中定义
 * @author 45degree
 * @see keymap.h
 * @since 0.0.1
 */
// typedef struct key_processing {
//    void (*process_displayable_key)(u32 key);     //!< 处理可显示字符
//    void (*process_not_displayable_key)(u32 key); //!< 处理不可显示字符
//} KEY_PROCESSING;

/**
 * @brief 键盘中断处理函数
 * @param[in] irq 中端号, 终端发生时自动压入
 * @author 45degree
 * @since 0.0.1
 */
void keyboard_handler(int irq);

/**
 * @brief 初始化键盘中断
 * @author 45degree
 * @since 0.0.1
 */
void init_keyboard();

/**
 * @brief 从键盘缓冲区读取扫描码并交给相应的tty处理
 * @param tty tty缓冲区
 * @author 45degree
 * @since 0.0.1
 */
void keyboard_read(TTY *tty);

#ifdef __cplusplus
};
#endif

#endif