#ifndef QOS_SYS_CALL_H
#define QOS_SYS_CALL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "process.h"

typedef void* system_call;

/**
 * @brief 
 * 
 * @return int 
 */
int sys_get_ticks();

/**
 * @brief 将字符串写入到进程对应的tty中
 *  
 * @param buf[in] 需要写入的字符串
 * @param len[in] 字符串的长度
 * @param unused 占位符, 未使用
 * @param proc[in] 进程
 * @author 45degree
 * @since 0.0.1
 * @note 该函数在汇编中调用
 */
void sys_write(char* buf, int len, char* unused, PROCESS* proc);

/**
 * @brief 
 * 
 * @param __unused1 
 * @param __unused2 
 * @param s 
 * @param proc 
 * @return int 
 */
int sys_printx(int __unused1, int __unused2, char* s, PROCESS* proc);

int sys_sendrec(int function, int src_dest, MESSAGE* m, PROCESS* proc);

#ifdef __cplusplus
};
#endif

#endif // QOS_SYS_CALL_H
