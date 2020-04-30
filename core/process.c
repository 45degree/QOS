#include "process.h"
#include "packaging_asm32.h"

void schedule() {
    PROCESS *p;
    int greatest_ticks = 0;

    while (!greatest_ticks) {
        for (int i = 0; i < NR_TASKS; i++) {
            p = &proc_table[i];
            if (p->ticks > greatest_ticks) {
                greatest_ticks = p->ticks;
                // p_proc_ready = p;
                set_process_ready(p);
            }
        }
        if (!greatest_ticks) {
            for (int i = 0; i < NR_TASKS; i++) {
                p = &proc_table[i];
                p->ticks = p->priority;
            }
        }
    }
}
