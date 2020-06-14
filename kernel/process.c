#include "process.h"
#include "package_iA32/packaging_iA32.h"

PROCESS proc_table[NR_TASK + NR_PROC];
char task_stack[STACK_SIZE_TOTAL];

void schedule() {
    PROCESS *p;
    int greatest_ticks = 0;

    while (!greatest_ticks) {
        for (int i = 0; i < NR_TASK + NR_PROC; i++) {
            p = &proc_table[i];
            if(p->flags == 0) {
                if (p->ticks > greatest_ticks) {
                    greatest_ticks = p->ticks;
                    set_process_ready(p);
                }
            }
        }
        if (!greatest_ticks) {
            for (int i = 0; i < NR_TASK + NR_PROC; i++) {
                p = &proc_table[i];
                if(p->flags == 0) {
                    p->ticks = p->priority;
                }
            }
        }
    }
}

u32 ldt_seg_linear(PROCESS* p, int idx) {
    DESCRIPTOR* desc = &p->ldts[idx];
    return desc->base_high << 24 | desc->base_mid << 16 | desc->base_low;
}


u32 va2la(int pid, void* va) {
    PROCESS* proc = &proc_table[pid];
    u32 seg_base = ldt_seg_linear(proc, INDEX_LDT_RW);
    u32 la = seg_base + (u32)va;
    return la;
}