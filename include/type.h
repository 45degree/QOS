/**
 * @file
 * @brief
 * @author  degree45
 * @date    2020/10/2
 * @version 0.0.1
 */

#ifndef TYPE_H
#define TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef va_arg
typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, l) __builtin_va_arg(v, l)
#endif

/**
 * @brief get the length of registers in different CPU
 *
 * if the programming is running on the 32-Bit computer,
 * the size of `op_t` is 4, and if the programming is running
 * on the 64-Bit computers, the size of `op_t` is 8.
 */
typedef unsigned long op_t;

typedef unsigned long addr_t;

typedef unsigned int u32;

typedef int i32;

typedef unsigned long long u64;

typedef long long i64;

typedef unsigned short u16;

typedef short i16;

typedef unsigned char u8;

typedef char i8;

#ifdef __cplusplus
};
#endif

#endif
