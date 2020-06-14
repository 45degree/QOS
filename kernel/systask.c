#include "message.h"
#include "process.h"
#include "panic.h"
#include "clock.h"
#include "printk.h"

void task_sys() {
    MESSAGE msg;
    while(1) {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch (msg.type) {
        case GET_TICKS:
            msg.RETVAL = ticks;
            send_recv(SEND, src, &msg);
            break;
        default:
            panic("unknown msg type");
            break;
        }
    }
}