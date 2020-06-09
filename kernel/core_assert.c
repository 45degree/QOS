#include "printk.h"
#include "core_assert.h"

static void spin(char* function_name) {
    printk("\n spinning in function %s", function_name);
    while(1);
}

void assert_failure(char* exp, char* file, char* basefile, int line) {
    printk("%c_assert (%s) failed: file: %s, base file: %s, ln %x", MAG_CH_ASSERT, exp, file, basefile, line);

    spin("assertion failure()");

    __asm__ __volatile__("ud2");
}