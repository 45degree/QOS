#ifndef QOS_TTY_H
#define QOS_TTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "console.h"

#define TTY_IN_BYTE 256 //!< tty输入队列大小
#define NR_CONSOLES 3   //!< tty数量

/**
 * tty 用于存放可显示字符
 */
typedef struct tty {
    u32 in_buf[TTY_IN_BYTE + 1];
    int head;
    int tail;
    int count;
    CONSOLE console;
} TTY;

TTY* current_tty;

/**
 * @brief 从tty中读入一个key
 * @param tty 指定的tty
 * @return 读取的key
 * @note 如果读取错误则返回0
 * @author 45degree
 * @since 0.0.1
 */
u32 tty_read(TTY* tty);

/**
 * @brief 向一个tty中写入key
 * @param tty 指定的tty
 * @param ch  写入的key
 * @return 0 写入失败, 1 写如成功
 * @author 45degree
 * @since 0.0.1
 */
int tty_write(TTY* tty, u32 ch);

/**
 * @brief 切换tty
 * @param num 需要切换到的tty序号
 * @author 45degree
 * @since 0.0.1
 */
void change_tty(int num);

void tty_show(TTY* tty);

void tty_put_char(TTY* tty);

void task_tty();

TTY tty_table[NR_CONSOLES]; //!< 所有的tty任务

#ifdef __cplusplus
};
#endif

#endif