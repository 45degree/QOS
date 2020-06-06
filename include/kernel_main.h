#ifndef QOS_MAIN_H
#define QOS_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "process.h"

extern int kernel_main();

extern TASK task_table[];

#ifdef __cplusplus
};
#endif

#endif
