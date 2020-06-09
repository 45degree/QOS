#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

#define SENDING   0x02	/* set when proc trying to send */
#define RECEIVING 0x04	/* set when proc trying to recv */
#define INVALID_DRIVER -20
#define INTERRUPT -10

#define TASK_TTY 0
#define TASK_SYS 1

#define SEND 1
#define RECEIVE 2
#define BOTH 3 /* BOTH = (SEND | RECEIVE) */

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

typedef struct {
    int source;
    int type;
    union {
        struct mess1 m1;
        struct mess2 m2;
        struct mess3 m3;
    } u;
} MESSAGE;

enum msgtype {
	HARD_INT = 1,
	GET_TICKS,
};

typedef struct s_proc PROCESS;

int msg_send(PROCESS* current, int dst, MESSAGE* m);
int msg_receive(PROCESS* current, int src, MESSAGE* m);
void reset_msg(MESSAGE* msg);
int send_recv(int function, int src_dest, MESSAGE* msg);

#define	RETVAL u.m3.m3i1

#ifdef __cplusplus
};
#endif

#endif