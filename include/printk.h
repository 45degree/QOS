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
int printk(const char* fmt, ...);

/**
 * @brief 格式化字符串
 * @param[out] buf 输出的格式化后字符串
 * @param[in]  fmt 格式化字符串
 * @param[in]  args 格式化的输出字符
 * @return 最后一个格式化字符的结尾位置
 * @author 45degree
 * @since 0.01
 * @note 暂时只支持"%x %c %s"的格式化
 */
int vsprintk(char* buf, const char* fmt, char* args);

#ifdef __cplusplus
};
#endif

#endif // QOS_PRINTK_H
