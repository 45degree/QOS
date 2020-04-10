#include <proc.h>

PROCESS proc_table[NR_TASKS];
char    task_stack[STACK_SIZE_TOTAL];
PROCESS* p_proc_ready;

extern void TestA();
extern int kernel_main();
extern void delay(int time);

extern void restart();