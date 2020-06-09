//
// Created by qijinwen on 5/13/20.
//
#include "sys_call.h"
#include "clock.h"
#include "tty/tty.h"
#include "show.h"
#include "core_assert.h"
#include "process.h"
#include "message.h"

system_call sys_call_table[] = {sys_get_ticks, sys_write, sys_sendrec, sys_printx};

int sys_get_ticks() { return ticks; }

void sys_write(char* buf, int len, char* _unused, PROCESS* proc) {
    for(int i = 0; i < len; i++) {
        char c = *buf;
        console_output_char(&(tty_table[proc->tty].console), c);
        ++buf;
    }
}

int sys_sendrec(int function, int src_dest, MESSAGE* m, PROCESS* proc) {
    core_assert(k_reenter == 0);
    core_assert((src_dest >= 0 && src_dest < NR_TASK + NR_PROC) || src_dest == ANY || src_dest == INTERRUPT);
    int ret = 0;
    int caller = proc2pid(proc);
    MESSAGE* mla = (MESSAGE*) va2la(caller, m);
    mla->source = caller;

    core_assert(mla->source != src_dest);

    if(function == SEND) {
        ret = msg_send(proc, src_dest, m);
        if(ret != 0) return ret;
    }
    else if (function == RECEIVE) {
        ret = msg_receive(proc, src_dest, m);
        if(ret != 0) return ret;
    }
    else {
        panic("{sys_sendrec} invalid. function: \"%d (SEND: %d, RECEIVE: %d)\" ", function, SEND, RECEIVE);
    }
    return 0;
}

int sys_printx(int __unused1, int __unused2, char* s, PROCESS* proc) {
    const char* p;
    char ch;
    char* reenter_err = "? k_reenter is incorrect for unknown reason";
    reenter_err[0] = MAG_CH_PANIC;

    if (k_reenter == 0) p = va2la(proc2pid(proc), s);
    else if (k_reenter > 0) p = s;
    else p = reenter_err;

    if((*p == MAG_CH_PANIC) || (*p == MAG_CH_ASSERT && p_proc_ready < &proc_table[NR_TASK])) {
        disable_int();
        char* v = (char*)V_MEM_BASE;
        const char* q = p + 1;
        while(v < (char*)(V_MEM_BASE + V_MEM_SIZE)) {
            *v++ = *q++;
            *v++ = RED_CHAR;
            if(!*q) {
                while(((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16)) {
                    v++;
                    *v++ = GRAY_CHAR;
                }
                q = p + 1;
            }
        }

        __asm__ __volatile__("hlt");
    }

    while((ch = *p++) != 0) {
        if(ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT) continue;
        console_output_char(&tty_table[proc->tty].console, ch);
    }
    return 0;
}

int get_ticks() {
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}