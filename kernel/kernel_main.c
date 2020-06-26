#include "kernel_main.h"
#include "clock.h"
#include "keyboard/keyboard.h"
#include "package_iA32/packaging_iA32.h"
#include "process.h"
#include "show.h"
#include "tty/tty.h"
#include "systask.h"
#include "hd.h"

void testA();

TASK task_table[NR_TASK] = {{task_tty, STACK_SIZE_TESTA, "task_tty"},
    {task_sys, STACK_SIZE_TESTA, "SYS"},
    {task_hd, STACK_SIZE_TESTA, "TASK HD"},
    {task_fs, STACK_SIZE_TESTA, "TASK FS"}};

TASK user_proc_table[NR_PROC] = {{testA, STACK_SIZE_TESTA, "testA"}};

int kernel_main() {
    display_str("-------\"kernel_main\" begins------");
    ticks = 0;
    TASK* p_task = task_table;
    PROCESS* p_proc = proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16 selector_ldt = selector_ldt_first;

    proc_table[0].ticks = proc_table[0].priority = 100;
    proc_table[1].ticks = proc_table[1].priority = 100;
    proc_table[2].ticks = proc_table[2].priority = 100;
    proc_table[3].ticks = proc_table[3].priority = 100;
    proc_table[4].ticks = proc_table[4].priority = 15;

    u8 privilege;
    u8 rpl;
    int eflags;
    for (int i = 0; i < NR_TASK + NR_PROC; i++) {
        if( i < NR_TASK ) {
            // 任务
            p_task = task_table + i;
            privilege = privilege_task;
            rpl = RPL_TASK;
            eflags = 0x1202; // IF = 1, IOPL = 1
        }
        else {
            // 用户进程
            p_task = user_proc_table + (i - NR_TASK);
            privilege = privilege_user;
            rpl = RPL_USER;
            eflags = 0x202; // IF = 1
        }
        core_strcpy(p_proc->p_name, p_task->name);
        p_proc->pid = i;
        p_proc->ldt_sel = selector_ldt;
        core_memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(struct descriptor));
        p_proc->ldts[0].attr1 = da_c | privilege << 5u;
        core_memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(struct descriptor));
        p_proc->ldts[1].attr1 = da_drw | privilege << 5;
        p_proc->regs.cs = (0u & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | rpl;
        p_proc->regs.ds = (8u & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | rpl;
        p_proc->regs.es = (8u & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | rpl;
        p_proc->regs.fs = (8u & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | rpl;
        p_proc->regs.ss = (8u & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | rpl;
        p_proc->regs.gs = (8u & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        p_proc->tty = 0;
        p_proc->flags = 0;
        p_proc->msg = 0;
        p_proc->recvfrom = NO_TASK;
        p_proc->sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->sending = 0;
        p_proc->next_sending = 0;

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1u << 3u;
    }
    k_reenter = 0;

    proc_table[0].tty = 1;
    proc_table[1].tty = 2;
    proc_table[2].tty = 0;
    proc_table[3].tty = 0;
    proc_table[4].tty = 1;

    set_process_ready(proc_table);
    display_clear();
    init_keyboard();
    init_clock();
    restart();
    while (1) {}
    return 0;
};
