#ifndef SYSTASK_H
#define SYSTASK_H

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_TTY 0
#define TASK_SYS 1
#define TASK_HD  2

extern void init_hd();

extern void hd_handler();

extern void hd_identify(int driver);

extern void task_sys();

extern void task_hd();

extern void task_fs();
#ifdef __cplusplus
};
#endif

#endif
