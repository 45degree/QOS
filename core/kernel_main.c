#include "kernel_main.h"
#include "clock.h"
#include "keyboard.h"
#include "packaging_asm32.h"
#include "process.h"
#include "show.h"
#include "tty.h"

TASK task_table[NR_TASKS] = {{task_tty, STACK_SIZE_TESTA, "task_tty"}};

int kernel_main() {
    display_str("-------\"kernel_main\" begins------");
    ticks = 0;
    TASK* p_task = task_table;
    PROCESS* p_proc = proc_table;
    char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
    u16 selector_ldt = selector_ldt_first;

    proc_table[0].ticks = proc_table[0].priority = 15;

    for (int i = 0; i < NR_TASKS; i++) {
        core_strcpy(p_proc->p_name, p_task->name);
        p_proc->pid = i;
        p_proc->ldt_sel = selector_ldt;
        core_memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = da_c | privilege_task << 5;
        core_memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = da_drw | privilege_task << 5;
        p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | RPL_TASK;
        p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | RPL_TASK;
        p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | RPL_TASK;
        p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | RPL_TASK;
        p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | RPL_TASK;
        p_proc->regs.gs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | sa_TIL | RPL_TASK;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = 0x1202;

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }
    k_reenter = 0;

    // p_proc_ready = proc_table;
    set_process_ready(proc_table);

    display_clear();
    init_keyboard();
    init_clock();
    restart();
    while (1) {}
    return 0;
}
