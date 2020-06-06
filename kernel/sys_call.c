//
// Created by qijinwen on 5/13/20.
//
#include "sys_call.h"
#include "clock.h"
#include "tty/tty.h"
#include "show.h"

system_call sys_call_table[] = {sys_get_ticks, sys_write};

int sys_get_ticks() { return ticks; }

void sys_write(char* buf, int len, char* _unused, PROCESS* proc) {
    for(int i = 0; i < len; i++) {
        char c = *buf;
        console_output_char(&(tty_table[proc->tty].console), c);
        ++buf;
    }
}