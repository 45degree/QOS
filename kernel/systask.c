#include "systask.h"
#include "message.h"
#include "process.h"
#include "panic.h"
#include "clock.h"
#include "printk.h"
#include "hd.h"

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

void task_hd() {
    MESSAGE msg;
    init_hd();

    while(1) {
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch(msg.type) {
        case DEV_OPEN:
            hd_identify(0);
            break;
        default:
            printk("HD ERROR");
            while(1);
            break;
        }

        send_recv(SEND, src, &msg);
    }
}

void task_fs() {
    printk("TASK FS begin.\n");

    MESSAGE driver_msg;
    driver_msg.type = DEV_OPEN;
    send_recv(BOTH, TASK_HD, &driver_msg);
    printk("FS");
    while(1);
}
