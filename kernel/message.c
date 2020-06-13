#include "message.h"
#include "package_iA32/packaging_iA32.h"
#include "core_assert.h"
#include "process.h"
#include "printk.h"
#include "panic.h"

static void block(PROCESS* proc) {
    core_assert(proc->flags);
    schedule();
}

static void unblock(PROCESS* proc) {
    core_assert(proc->flags == 0);
}

static int deadlock(int src, int dest) {
    PROCESS* p = proc_table + dest;
    while(1) {
        if(p->flags & SENDING) {
            if(p->sendto == src) {
                p = proc_table + dest;
                printk("=_=%s", p->p_name);
                do {
                    core_assert(p->msg);
                    p = proc_table + p->sendto;
                    printk("->%s", p->p_name);
                }while(p != proc_table + src);
                printk("=_=");
                return 1;
            }
            p = proc_table + p->sendto;
        }
        else break;
    }
    return 0;
}

int msg_send(PROCESS* current, int dst, MESSAGE* m) {
    PROCESS* sender = current;
    PROCESS* dest = proc_table + dst;

    core_assert(proc2pid(sender) != dst);

    if(deadlock(proc2pid(sender), dest)) {
        panic(">>DEADLOCK<< %s -> %s", sender->p_name, dest->p_name);
    }

    if((dest->flags & RECEIVING) && (dest->recvfrom == proc2pid(sender) || dest->recvfrom == ANY)) {
        core_assert(dest->msg);
        core_assert(m);
        core_memcpy(va2la(dst, dest->msg), va2la(proc2pid(sender), m), sizeof(MESSAGE));
        dest->msg = 0;
        dest->flags &= ~RECEIVING;
        dest->recvfrom = NO_TASK;
        unblock(dest);

        core_assert(dest->flags == 0);
        core_assert(dest->msg == 0);
        core_assert(dest->recvfrom == NO_TASK);
        core_assert(dest->sendto == NO_TASK);
        core_assert(sender->flags == 0);
        core_assert(sender->msg == 0);
        core_assert(sender->recvfrom == NO_TASK);
        core_assert(sender->sendto == NO_TASK);
    }
    else {
        sender->flags |= SENDING;
        core_assert(sender->flags == SENDING);
        sender->sendto = dest;
        sender->msg = m;

        PROCESS* p;
        if(dest->sending) {
            p = dest->sending;
            while(p->next_sending) p = p->next_sending;
            p->next_sending = sender;
        }
        else {
            dest->sending = sender;
        }
        sender->next_sending = 0;
        block(sender);
        core_assert(sender->flags == SENDING);
        core_assert(sender->msg != 0);
        core_assert(sender->recvfrom == NO_TASK);
        core_assert(sender->sendto == dst);
    }
    return 0;
}

int msg_receive(PROCESS* current, int src, MESSAGE* m) {
    PROCESS* who_wanna_recv = current;
    PROCESS* from = 0;
    PROCESS* prev = 0;
    int copyok = 0;

    core_assert(proc2pid(who_wanna_recv) != src);

    if((who_wanna_recv->has_int_msg) && ((src == ANY) || (src == INTERRUPT))) {
        MESSAGE msg;
        reset_msg(&msg);
        msg.source = INTERRUPT;
        msg.type = HARD_INT;
        core_assert(m);
        core_memcpy(va2la(proc2pid(who_wanna_recv), m), &msg, sizeof(MESSAGE));

        who_wanna_recv->has_int_msg = 0;
        core_assert(who_wanna_recv->flags == 0);
        core_assert(who_wanna_recv->msg == 0);
        core_assert(who_wanna_recv->sendto == NO_TASK);
        core_assert(who_wanna_recv->has_int_msg == 0);

        return 0;
    }

    if(src == ANY) {
        if(who_wanna_recv -> sending) {
            from = who_wanna_recv->sending;
            copyok = 1;

            core_assert(who_wanna_recv->flags == 0);
            core_assert(who_wanna_recv->msg == 0)
            core_assert(who_wanna_recv->recvfrom == NO_TASK);
            core_assert(who_wanna_recv->sendto == NO_TASK);
            core_assert(who_wanna_recv->sending != 0);
            core_assert(from->flags == SENDING);
            core_assert(from->msg != 0);
            core_assert(from->recvfrom == NO_TASK);
            core_assert(from->sendto == proc2pid(who_wanna_recv));
        }
    }
    else {
        from = &proc_table[src];
        if((from->flags & SENDING) && (from->sendto == proc2pid(who_wanna_recv))) {
            copyok = 1;

            PROCESS* p = who_wanna_recv->sending;
            core_assert(p);

            while(p) {
                core_assert(from->flags & SENDING);
                if(proc2pid(p) == src) {
                    from = p;
                    break;
                }
                prev = p;
                p = p->next_sending;
            }

            core_assert(who_wanna_recv->flags == 0);
            core_assert(who_wanna_recv->msg == 0)
            core_assert(who_wanna_recv->recvfrom == NO_TASK);
            core_assert(who_wanna_recv->sendto == NO_TASK);
            core_assert(who_wanna_recv->sending != 0);
            core_assert(from->flags == SENDING);
            core_assert(from->msg != 0);
            core_assert(from->recvfrom == NO_TASK);
            core_assert(from->sendto == proc2pid(who_wanna_recv));
        }
    }

    if(copyok) {
        if(from == who_wanna_recv->sending) {
            core_assert(prev == 0);
            who_wanna_recv->sending = from->next_sending;
            from->next_sending = 0;
        }
        else {
            core_assert(prev);
            prev->next_sending = from->next_sending;
            from->next_sending = 0;
        }
        core_assert(m);
        core_assert(from->msg);
        core_memcpy(va2la(proc2pid(who_wanna_recv), m), va2la(proc2pid(from), from->msg), sizeof(MESSAGE));
        from->msg = 0;
        from->sendto = NO_TASK;
        from->flags &= ~SENDING;
        unblock(from);
    }
    else {
        who_wanna_recv->flags |= RECEIVING;
        who_wanna_recv->msg = m;
        if(src == ANY) who_wanna_recv->recvfrom = ANY;
        else who_wanna_recv->recvfrom = proc2pid(from);

        block(who_wanna_recv);

        core_assert(who_wanna_recv->flags == RECEIVING);
        core_assert(who_wanna_recv->msg != 0);
        core_assert(who_wanna_recv->recvfrom != NO_TASK);
        core_assert(who_wanna_recv->sendto == NO_TASK);
        core_assert(who_wanna_recv->has_int_msg == 0);
    }
    return 0;
}

void reset_msg(MESSAGE* msg) {
    core_memset(msg, 0, sizeof(MESSAGE));
}

int send_recv(int function, int src_dest, MESSAGE* msg) {
    int ret = 0;
    if(function == RECEIVE) core_memset(msg, 0, sizeof(MESSAGE));

    switch (function) {
    case BOTH:
        ret = sendrec(SEND, src_dest, msg);
        if(ret == 0) ret = sendrec(RECEIVE, src_dest, msg);
        break;
    case SEND:
    case RECEIVE:
        ret = sendrec(function, src_dest, msg);
        break;
    default:
        core_assert((function == BOTH) || (function == SEND) || (function == RECEIVE));
        break;
    }
    return ret;
}