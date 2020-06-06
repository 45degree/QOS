#include "printk.h"

static void spin(char* function_name) {
    printk("\n spinning in function %s", function_name);
    while(1);
}

void assert_failure(char* exp, char* file, char* basefile, int line) {
    printk("", "");
}