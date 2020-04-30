/**
 * @file
 * @brief   该文件声明一些全局性的功能,包括定义GDT,LDT以及其相关功能,内存地址之间的转换,内存复制等。
 * @author  45degree
 * @date    2020/4/18
 * @version 0.0.1
 */

#ifndef PROC_H
#define PROC_H

#include "global.h"

/**
 * @brief 一个进程使用的寄存器的值
 */
typedef struct s_stackframe {
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 kernel_esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 retaddr;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} STACK_FRAME;

/* 进程结构 */
typedef struct s_proc {
    STACK_FRAME regs;          //!< 该进程使用的寄存器的值
    u16 ldt_sel;               //!< 指向ldt基址和界限的GDT选择子
    DESCRIPTOR ldts[LDT_SIZE]; //!< 该进程数据段和代码段的ldt局部描述符
    int ticks;                 //!< 进程的时间计数器
    int priority;              //!< 进程的优先级
    u32 pid;                   //!< 进程pid
    char p_name[16];           //!< 进程名
} PROCESS;

typedef void (*test_f)();

typedef struct s_task {
    test_f initial_eip;
    int stacksize;
    char name[32];
} TASK;

/**
 * @brief 进程调度函数
 */
extern void schedule();

PROCESS proc_table[NR_TASKS];      //!< 进程列表, 存放所有要运行的进程
char task_stack[STACK_SIZE_TOTAL]; //!< 进程堆栈

#endif