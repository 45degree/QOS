//
// Created by qijinwen on 5/13/20.
//

#include "printk.h"
#include "package_iA32/packaging_iA32.h"
#include "clock.h"
#include "core_assert.h"
#include "sys_call.h"

void testA() {
    int i = 0;
    while(1) {
        printk("<Ticks:%x>", get_ticks());
        milli_delay(200);
    }
}