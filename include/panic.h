#ifndef PANIC_H
#define PANIC_H

#ifdef __cplusplus
extern "C" { 
#endif

void panic(const char* fmt, ...);

#ifdef __cplusplus
};
#endif

#endif