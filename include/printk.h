//
// Created by qijinwen on 5/13/20.
//

#ifndef QOS_PRINTK_H
#define QOS_PRINTK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 格式化打印, 类似于printf
 * @param[in] fmt 格式化字符串
 * @author 45degree
 * @since 0.01
 */
int printk(const char* fmt, char* arg);

#ifdef __cplusplus
};
#endif

#endif // QOS_PRINTK_H
