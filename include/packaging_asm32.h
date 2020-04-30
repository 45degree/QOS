/**
 * @file
 * @brief
 * 该文件是对所有汇编函数以及所需要变量的一份声明
 * @author  45degree
 * @date    2020/4/28
 * @version 0.0.1
 */

#ifndef PACKAGING_ASM_32_H
#define PACKAGING_ASM_32_H

#include "global.h"
#include "process.h"

/**
 * @brief 得到屏幕的显示位置
 * @return 屏幕的显示位置
 * @author 45degree
 * @since 0.0.1
 */
extern int get_display_pose();

/**
 * @brief 设置屏幕的显示位置
 * @param pose[in] 屏幕的显示位置
 * @author 45degree
 * @since 0.0.1
 */
extern void set_display_pose(int pose);

/**
 * @brief 获取gdt_ptr中的值
 * @return 指向gdt_ptr的指针
 * @author 45degree
 * @since 0.0.1
 */
extern u8* get_gdt_ptr();

/**
 * @brief 获取idt_ptr中的值
 * @return 指向idt_ptr的指针
 * @author 45degree
 * @since 0.0.1
 */
extern u8* get_idt_ptr();

/**
 * @brief 获取TSS
 * @return 指向TSS的指针
 * @author 45degree
 * @since 0.0.1
 */
extern TSS* get_tss();

/**
 * @brief 初始化tss
 * @author 45degree
 * @since 0.0.1
 */
extern void init_tss();

/**
 * @brief 得到下一个要运行的进程的指针
 * @return 下一个要运行的进程的指针
 * @author 45degree
 * @since 0.0.1
 */
extern const PROCESS* get_process_ready();

/**
 * @brief 设置下一个要运行的进程的指针
 * @param process 一个要运行的进程的指针
 * @author 45degree
 * @since 0.0.1
 */
extern void set_process_ready(PROCESS* process);

//************************************* kernel.asm中的函数及变量声明 ************************************//

/**
 * @brief 开始执行下一个用户进程
 * @author 45degree
 * @note
 * 该函数定义在汇编代码中
 * @since 0.0.1
 */
extern void restart();

PROCESS* p_proc_ready; //!< 指向下一个要运行的进程
u32 k_reenter;         //!< 中断重入判断标志

//********************************** common32_lib.asm中的函数及变量声明 *********************************//

/**
 * @brief  复制指定大小的内存
 * @author 45degree
 * @param[out]  pDst  目标内存地址
 * @param[in]   pSrc  源地址
 * @param[in]   iSize 需要复制的内存大小
 * @return 目标内存地址指针
 * @note
 * 该函数实现由汇编实现
 * @since 0.0.1
 */
extern void* core_memcpy(void* pDest, void* pSrc, int iSize);

/**
 * @brief  从内存指定位置开始复制字符(一个无符号字符)n次
 * @author 45degree
 * @param[out]  p_dst  目标内存地址
 * @param[in]   ch     指定字符
 * @param[in]   size   复制次数
 * @note
 * 该函数实现由汇编实现
 * @since 0.0.1
 */
extern void core_memset(void* p_dst, char ch, int size);

/**
 * @brief  字符串复制
 * @author 45degree
 * @param[out]  p_dst  目标字符串
 * @param[in]   p_src  源字符串
 * @note
 * 该函数实现由汇编实现
 * @since 0.0.1
 */
extern void core_strcpy(char* p_dst, char* p_src);

/**
 * @brief 向端口写数据
 * @author 45degree
 * @param part  端口号
 * @param value 写入的数据值
 * @since 0.0.1
 */
extern void out_byte(u16 port, u8 value);

/**
 * @brief 从端口读数据
 * @author 45degree
 * @param part  端口号
 * @return 读出的数据
 * @since 0.0.1
 */
extern u8 in_byte(u16 port);

//******************************* interrupt32_lib.asm中的函数及变量声明 *********************************//

/**
 * @brief 关闭中断接受
 */
extern void disable_int();

/**
 * @brief 打开中断接受
 */
extern void enable_int();

extern void hwint00();
extern void hwint01();
extern void hwint02();
extern void hwint03();
extern void hwint04();
extern void hwint05();
extern void hwint06();
extern void hwint07();
extern void hwint08();
extern void hwint09();
extern void hwint15();
extern void hwint10();
extern void hwint11();
extern void hwint12();
extern void hwint13();
extern void hwint14();

extern void divide_error();
extern void single_step_exception();
extern void nmi();
extern void breakpoint_exception();
extern void overflow();
extern void bounds_check();
extern void inval_opcode();
extern void copr_not_avaliable();
extern void double_fault();
extern void copr_seg_overrun();
extern void inval_tss();
extern void segment_not_present();
extern void stack_exception();
extern void general_protection();
extern void page_fault();
extern void copr_error();

//****************************** displayText32_lib.asm中的函数及变量声明 ********************************//

extern void _display_str(const char* str);
extern void _display_color_str(const char* str, int TextColor);

int display_pose;      //!< 屏幕显示位置

//******************************** syscall32_lib.asm中的函数及变量声明 **********************************//

extern void sys_call();

extern int get_ticks();


//共6个字节，低两个字节保存的是gdt的最大偏移(长度-1)，高4字节保存gdt的起始物理地址
u8 gdt_ptr[6]; //!< GDTR寄存器中的值
u8 idt_ptr[6]; //!< IDTR寄存器中的值
TSS tss;       //!< tss

#endif
