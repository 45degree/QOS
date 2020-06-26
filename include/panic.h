#ifndef PANIC_H
#define PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAG_CH_PANIC '\002'

extern void panic(const char* fmt, ...);

#ifdef __cplusplus
};
#endif

#endif
