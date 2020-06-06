#include "process.h"
#include "package_iA32/packaging_iA32.h"

void schedule() {
    PROCESS *p;
    int greatest_ticks = 0;

    while (!greatest_ticks) {
        for (int i = 0; i < NR_TASK + NR_PROC; i++) {
            p = &proc_table[i];
            if (p->ticks > greatest_ticks) {
                greatest_ticks = p->ticks;
                set_process_ready(p);
            }
        }
        if (!greatest_ticks) {
            for (int i = 0; i < NR_TASK + NR_PROC; i++) {
                p = &proc_table[i];
                p->ticks = p->priority;
            }
        }
    }
}
