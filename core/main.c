#include <main.h>


void TestA() {
    int i = 0;
    while(1) {
        disp_str("A");
        disp_int(i++);
        disp_str(".");
        delay(1);
    }
}

int kernel_main() {
    disp_str("-------\"kernel_main\" begins------");
    PROCESS* p_proc = proc_table;

    p_proc -> ldt_sel = SELECTOR_LDT_FIRST;
    core_memcpy(&p_proc -> ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
    p_proc -> ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
    core_memcpy(&p_proc ->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(DESCRIPTOR));
    p_proc -> ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;

    p_proc -> regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc -> regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc -> regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc -> regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc -> regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc -> regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
    p_proc -> regs.eip = (u32)TestA;
    p_proc -> regs.esp = (u32) task_stack + STACK_SIZE_TOTAL;
    p_proc -> regs.eflags = 0x1202;
    p_proc_ready = proc_table;
    restart();
    while(1) {}
}

void delay(int times) {
    for (int k = 0; k < times; k++) {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10000; j++) {}
        }
    }
}