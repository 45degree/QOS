#include "panic.h"
#include "printk.h"

void panic(const char *fmt, ...) {
    int i;
    char buf[256];
    char* arg = &fmt + 1;
    i = vsprintk(buf, fmt, arg);

    printk("%c !!panic!! %s", MAG_CH_PANIC, buf);

    __asm__ __volatile__("ud2");
}
