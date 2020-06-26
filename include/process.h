/**
 * @file
 * @brief   
 * @author  45degree
 * @date    2020/4/18
 * @version 0.0.1
 */

#ifndef QOS_PROC_H
#define QOS_PROC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"
#include "message.h"

#define NR_TASK 4
#define NR_PROC 1

#define ANY (NR_TASK + NR_PROC + 10)
#define NO_TASK (NR_TASK + NR_PROC + 20)

#define proc2pid(x) (x - proc_table)

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
    STACK_FRAME regs;            //!< 该进程使用的寄存器的值
    u16 ldt_sel;                 //!< 指向ldt基址和界限的GDT选择子
    struct descriptor ldts[LDT_SIZE];   //!< 该进程数据段和代码段的ldt局部描述符
    int ticks;                   //!< 进程的时间计数器
    int priority;                //!< 进程的优先级
    u32 pid;                     //!< 进程PID
    char p_name[16];             //!< 进程名
    int flags;                   //!< 进程是否可运行，flags=0代表可以运行
    MESSAGE* msg;                //!< 进程接受的消息
    int recvfrom;                //!< 该进程打算从哪一个进程(PID)中接受信息
    int sendto;                  //!< 该进程打算发送信息到哪一个进程(PID)
    int has_int_msg;             //!< 进程是否在等待中断发生
    struct s_proc* sending;      //!< 需要给该进程发送消息的进程队列中的第一个进程
    struct s_proc* next_sending; //!< 需要给该进程发送消息的进程队列中的下一个进程
    int tty;                     //!< 进程运行所在的tty
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

/**
 * @brief 计算进程某一段地址的基地址
 * 
 * @param p 进程
 * @param idx 段序号
 * @return u32 段地址
 */
extern u32 ldt_seg_linear(PROCESS* p, int idx);

u32 va2la(int pid, void* va);

extern PROCESS proc_table[NR_TASK + NR_PROC];      //!< 进程列表, 存放所有要运行的进程
extern char task_stack[STACK_SIZE_TOTAL]; //!< 进程堆栈

#ifdef __cplusplus
};
#endif

#endif
