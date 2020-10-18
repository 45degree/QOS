#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"
#include "process.h"

#define SENDING   0x02	/* set when proc trying to send */
#define RECEIVING 0x04	/* set when proc trying to recv */
#define INVALID_DRIVER -20
#define INTERRUPT -10

/**
 * @brief this is the enum of the message function,
 *
 */
enum msg_function {
    SEND = 1,
    RECEIVE,
    BOTH,
};

struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};

struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void* m3p1;
	void* m3p2;
};

struct message {
    int source;
    int type;
    union {
        struct mess1 m1;
        struct mess2 m2;
        struct mess3 m3;
    } u;
};

enum msgtype {
    HARD_INT = 1,
    GET_TICKS,

    DEV_OPEN = 1001,
    DEV_CLOSE,
    DEV_READ,
    DEV_WRITE,
    DEV_IOCTL
};

// typedef struct s_proc PROCESS;

int msg_send(struct process* current, int dst, struct message* msg);
int msg_receive(struct process* current, int src, struct message* msg);
void reset_msg(struct message* msg);
void inform_int(int task_nr);
int send_recv(int function, int src_dest, struct message* msg);

#define	RETVAL u.m3.m3i1

#ifdef __cplusplus
};
#endif

#endif
