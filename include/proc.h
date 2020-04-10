#ifndef _PROC_H
#define _PROC_H

#include <global.h>

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
}STACK_FRAME;

/* 进程结构 */
typedef struct s_proc {
    STACK_FRAME regs;             // 该进程使用的寄存器的值
    u16 ldt_sel;                  // 指向ldt基址和界限的GDT选择子
    DESCRIPTOR ldts[LDT_SIZE];    // 该进程数据段和代码段的局部描述符
    u32 pid;                      // 进程pid
    char p_name[16];              // 进程名
}PROCESS;

#endif