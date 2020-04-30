/**
 * @file
 * @brief   该文件定义一些中断的功能
 * @author  45degree
 * @date    2020/4/18
 * @version 0.0.1
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "global.h"

/*8259A 中断控制端口*/
#define INT_M_CTL 0x20     //!< 主8259A对应的端口1
#define INT_M_CTLMASK 0x21 //!< 主8259A对应的端口2
#define INT_S_CTL 0xA0     //!< 从8259A对应的端口1
#define INT_S_CTLMASK 0xA1 //!< 从8259A对应的端口2

/*中断控制向量*/
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

/* 中断向量(异常使用) */
enum exception_vector {
    int_vector_divide = 0x0, //!< 除法错误异常
    int_vector_debug,        //!< 调试异常
    int_vector_nmi,          //!< 非屏蔽中断
    int_vector_breakpoint,   //!< 调试断点
    int_vector_overflow,     //!< 溢出
    int_vector_bound,        //!< 越界
    int_vector_inval_op,     //!< 无效操作码
    int_vector_coproc_not,   //!< 设备不可用
    int_vector_double_fault, //!< 双重错误
    int_vector_coproc_seg,   //!< 协处理器段越界
    int_vector_inval_tss,    //!< 无效TSS
    int_vector_seg_not,      //!< 段不存在
    int_vector_stack_fault,  //!< 堆栈段错误
    int_vector_protection,   //!< 常规保护错误
    int_vector_page_fault,   //!< 页错误
    int_vector_coproc_err    //!< x87FPU浮点错误
};

/* 外部可屏蔽中断 */
#define NR_IRQ 16       //!< 外部可屏蔽中断一共有16个
#define CLOCK_IRQ 0     //!< 时钟中断
#define KEYBOARD_IRQ 1  //!< 键盘中断
#define CASCADE_IRQ 2   //!< cascade enable for 2nd AT controller
#define ETHER_IRQ 3     //!< default ethernet interrupt vector
#define SECONDARY_IRQ 3 //!< RS232 interrupt vector for port 2
#define RS232_IRQ 4     //!< RS232 interrupt vector for port 1
#define XT_WINI_IRQ 5   //!< xt winchester
#define FLOPPY_IRQ 6    //!< floppy disk
#define PRINTER_IRQ 7   //!<
#define AT_WINI_IRQ 14  //!< at winchester

#define INT_VECTOR_SYS_CALL 0x90 //!< 系统调用中断

/**
 * @brief  异常和中断处理函数指针
 * @author 45degree
 *
 * 该函数指针主要用于中断和异常的处理函数注册
 * @since 0.0.1
 * @see init_idt_desc
 */
typedef void (*int_handler)();

/**
 * @brief  外部可屏蔽中断处理函数指针
 * @author 45degree
 *
 * 该函数指针用于外部可屏蔽中断的处理函数注册, 当外部中断发生时,
 * 首先进入int_handler指向的处理函数, 之后在处理函数内部调用irq_handle指向的函数。
 * @note 具体的调用过程可以查看kernel.asm中的hwint_master宏定义
 * @since 0.0.1
 * @see int_handler
 */
typedef void (*irq_handle)(int irq);

irq_handle irq_table[NR_IRQ]; //!< 外部可屏蔽中断处理函数表, 用于在汇编代码中找到正确的处理函数

/**
 * @brief  异常处理函数
 * @author 45degree
 * @param[in] vec_no   中断向量
 * @param[in] err_code 异常错误码
 * @param[in] eip      异常发生时的eip寄存器的值
 * @param[in] cs       异常发生时的cs寄存器的值
 * @param[in] eflags   异常发生时的eflags寄存器的值
 * @note
 * 该函数在汇编代码中调用, 其中eip, cs, eflags会在调用中断门时自动的压栈
 * @since 0.0.1
 */
extern void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);

/**
 * @brief  屏蔽某一外部中断
 * @author 45degree
 * @param[in] irq 外部中断号
 * @since 0.0.1
 */
extern void disable_irq(int irq);

/**
 * @brief  启动某一外部中断
 * @author 45degree
 * @param[in] irq 外部中断号
 * @since 0.0.1
 */
extern void enable_irq(int irq);

/**
 * @brief  设置某一外部中断的处理函数
 * @author 45degree
 * @param[in] irq 外部中断号
 * @param[in] handler 外部中断处理函数指针
 * @since 0.0.1
 */
extern void put_irq_handler(int irq, irq_handle handler);

/**
 * @brief  初始化中断和异常对应的端口
 * @author 45degree
 * @since 0.0.1
 */
extern void init_prot();

/**
 * @brief  初始化8259A
 * @author 45degree
 * @since 0.0.1
 */
extern void init_8259A();

/**
 * @brief  一般通用的外部中断处理函数
 * @author 45degree
 * @param[in]  irq 外部中断号
 * @since 0.0.1
 */
extern void spurious_irq(int irq);

#endif
