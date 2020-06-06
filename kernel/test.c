//
// Created by qijinwen on 5/13/20.
//

#include "printk.h"
#include "package_iA32/packaging_iA32.h"
#include "clock.h"

void testA() {
    int i = 0;
    while(1) {
        printk("<Ticks:%x>", 12,13,14,15,16,17,18);
        milli_delay(200);
    }
}