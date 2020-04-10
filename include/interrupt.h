#ifndef INTERRUPT_MACRO_H
#define INTERRUPT_MACRO_H

#include "global.h"

/*8259A 中断控制端口*/
#define INT_M_CTL               0x20
#define INT_M_CTLMASK           0x21
#define INT_S_CTL               0xA0
#define INT_S_CTLMASK           0xA1

/*中断控制向量*/
#define INT_VECTOR_IRQ0         0x20
#define INT_VECTOR_IRQ8         0x28

/* 中断向量 */
#define	INT_VECTOR_DIVIDE       0x0
#define	INT_VECTOR_DEBUG        0x1
#define	INT_VECTOR_NMI          0x2
#define	INT_VECTOR_BREAKPOINT   0x3
#define	INT_VECTOR_OVERFLOW     0x4
#define	INT_VECTOR_BOUNDS       0x5
#define	INT_VECTOR_INVAL_OP     0x6
#define	INT_VECTOR_COPROC_NOT   0x7
#define	INT_VECTOR_DOUBLE_FAULT 0x8
#define	INT_VECTOR_COPROC_SEG   0x9
#define	INT_VECTOR_INVAL_TSS    0xA
#define	INT_VECTOR_SEG_NOT      0xB
#define	INT_VECTOR_STACK_FAULT  0xC
#define	INT_VECTOR_PROTECTION   0xD
#define	INT_VECTOR_PAGE_FAULT   0xE
#define	INT_VECTOR_COPROC_ERR   0x10

typedef void(* int_handler)();

extern void out_byte(u16 port, u8 value);
extern void in_byte(u16 port);

extern void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags);
extern void init_prot();
extern void init_8259A();

extern void spurious_irq(int irq);

/*以下为硬件中断处理程序*/
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


/*以下异常处理程序代码在汇编中定义*/
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

#endif
