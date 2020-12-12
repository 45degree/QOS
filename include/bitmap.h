/**
 * @file
 * @brief
 * @author  degree45
 * @date    2020/10/2
 * @version 0.0.1
 */
#ifndef _QOS_BITMAP_H_
#define _QOS_BITMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

#define BITS_PRE_BYTE 8

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

/**
 * @brief this macro is used to calculate the length of a array
 *        which you want to contain some bits
 *
 * @param nr the number of bits that you want the array contains
 *
 * @note
 *     the type of the array is determined by your compiler flags:
 *         - if you compile in 32-bits,
 *           the size of the elements in array is 4 bytes;
 *         - if you compile in 64-bits,
 *           the size of the elements in array is 8 bytes;
 *
 * @see DECLARE_BITMAP
 */
#define BITS_TO_ARRAY_LEN(nr) DIV_ROUND_UP(nr, BITS_PRE_BYTE * sizeof(op_t))

/**
 * @brief this macro is used to declare a bitmap
 *
 * @param name the bitmap's name
 * @param bits the count of the bits in the bitmap
 */
#define DECLARE_BITMAP(name, bits) op_t name[BITS_TO_ARRAY_LEN(bits)] = {0};

/**
 * @brief set the bit to one in the bitmap
 * @param bitmap the bitmap that you want to manipulate
 * @param num the position of the bit in the bitmap
 *
 * @note
 *     the parameter `num` starts from 0.
 *
 * @attention
 *     if `num` is larger than the length of bitmap,
 *     the function will not insure that this is a safe operation.
 */
static inline void set_bits(op_t* bitmap, int num) {
    op_t mask = 1UL << num % (sizeof(op_t) * BITS_PRE_BYTE);
    op_t* tmp = bitmap + num / (sizeof(op_t) * BITS_PRE_BYTE);
    *tmp |= mask;
    return;
}

/**
 * @brief set the bit to zero in the bitmap
 * @param bitmap the bitmap that you want to manipulate
 * @param num the position of the bit in the bitmap
 *
 * @note
 *     the parameter `num` starts from 0.
 *
 * @attention
 *     if `num` is larger than the length of bitmap,
 *     the function will not insure that this is a safe operation.
 */
static inline void clear_bits(op_t* bitmap, int num) {
    op_t mask = ~(1UL << num % (sizeof(op_t) * BITS_PRE_BYTE));
    op_t* tmp = bitmap + num / (sizeof(op_t) * BITS_PRE_BYTE);
    *tmp &= mask;
    return;
}

#ifdef __cplusplus
};
#endif

#endif
