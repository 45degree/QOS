//
// Created by qijinwen on 5/13/20.
//

#include "printk.h"
#include "package_iA32/packaging_iA32.h"
#include "clock.h"

void testA() {
    int i = 0;
    while(1) {
        printk("<Ticks:%s>", "dsadas");
        milli_delay(200);
    }
}