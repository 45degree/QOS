#include "interrupt.h"
#include "package_iA32/packaging_iA32.h"
#include "process.h"
#include "show.h"

static const char *error_msg[] = {"#DE Divide Error",
                                  "#DB RESERVED",
                                  "--  NMI Interrupt --",
                                  "#BP Breakpoint",
                                  "#OF Overflow",
                                  "#BR BOUND Range Exceeded",
                                  "#UD Invalid Opcode (Undefined Opcode)",
                                  "#NM Device Not Available (No Math Coprocessor)",
                                  "#DF Double Fault",
                                  "Coprocessor Segment Overrun (reserved)",
                                  "#TS Invalid TSS",
                                  "#NP Segment Not Present",
                                  "#SS Stack-Segment Fault",
                                  "#GP General Protection",
                                  "#PF Page Fault",
                                  "--  (Intel reserved. Do not use.)  --",
                                  "#MF x87 FPU Floating-Point Error (Math Fault)",
                                  "#AC Alignment Check",
                                  "#MC Machine Check",
                                  "#XF SIMD Floating-Point Exception"};

irq_handle irq_table[NR_IRQ];

/**
 * 初始化i386中断门
 */
static void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler,
                          enum privilege privilege) {
    struct gate *p_gate = &idt[vector];
    u32 base = (u32)handler;
    p_gate->offset_low = base & 0xFFFF;
    p_gate->selector = SELECTOR_KERNEL_CS;
    p_gate->dcount = 0;
    p_gate->attr = desc_type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xFFFF;
}

/**
 * 初始化8259A芯片, 打开时钟中断, 并设置外部中断的中断默认处理函数
 * @note 设置了中断的处理函数，但是并没有使中断生效
 */
void init_8259A() {
    out_byte(INT_M_CTL, 0x11);
    out_byte(INT_S_CTL, 0x11);
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0);
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8);
    out_byte(INT_M_CTLMASK, 0x4);
    out_byte(INT_S_CTLMASK, 0x2);
    out_byte(INT_M_CTLMASK, 0x1);
    out_byte(INT_S_CTLMASK, 0x1);

    // 打开时钟中断
    out_byte(INT_M_CTLMASK, 0xFE);
    out_byte(INT_S_CTLMASK, 0xFF);

    // 设置外部中断的中断默认处理函数
    for (int i = 0; i < NR_IRQ; i++) { irq_table[i] = spurious_irq; }
}

/**
 * 在外部可屏蔽中断处理函数表中设置相应的处理函数
 * @note 在设置了处理函数的同时屏蔽了相应的中断, 如要打开执行enable_irq
 * @see irq_table enable_irq disable_irq
 */
void put_irq_handler(int irq, irq_handle handler) {
    disable_irq(irq);
    irq_table[irq] = handler;
}

void disable_irq(int irq) {
    if (irq < 8)
        out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) | (1u << irq));
    else
        out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) | (1u << (irq % 8)));
}

void enable_irq(int irq) {
    if (irq < 8)
        out_byte(INT_M_CTLMASK, in_byte(INT_M_CTLMASK) & ~(1u << irq));
    else {
        out_byte(INT_S_CTLMASK, in_byte(INT_S_CTLMASK) & ~(1u << (irq % 8))); 
    }
}

/**
 * 通用外部中断处理函数, 在屏幕中打印相应的外部中断号
 */
void spurious_irq(int irq) {
    display_str("spurious irq: ");
    display_int(irq);
    display_str("\n");
}

void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags) {
    int text_color = 0x74;
    set_display_pose(0);
    for (int i = 0; i < 80 * 5; i++) { display_str(" "); }
    set_display_pose(0);

    display_color_str("Exception! -->", text_color);
    display_color_str(error_msg[vec_no], text_color);
    display_color_str("\n\n", text_color);
    display_color_str("EFLAGS:", text_color);
    display_int(eflags);
    display_color_str("CS:", text_color);
    display_int(cs);
    display_color_str("EIP:", text_color);
    display_int(eip);

    if (err_code != 0xFFFFFFFF) {
        display_color_str("ERROR CODE:", text_color);
        display_int(err_code);
    }
}

void init_port() {
    init_8259A();

    // 为异常设置相应的处理程序
    init_idt_desc(int_vector_divide, da_i_gate_386, divide_error, privilege_kernel);
    init_idt_desc(int_vector_debug, da_i_gate_386, single_step_exception, privilege_kernel);
    init_idt_desc(int_vector_nmi, da_i_gate_386, nmi, privilege_kernel);
    init_idt_desc(int_vector_breakpoint, da_i_gate_386, breakpoint_exception, privilege_kernel);
    init_idt_desc(int_vector_overflow, da_i_gate_386, overflow, privilege_kernel);
    init_idt_desc(int_vector_bound, da_i_gate_386, bounds_check, privilege_kernel);
    init_idt_desc(int_vector_inval_op, da_i_gate_386, inval_opcode, privilege_kernel);
    init_idt_desc(int_vector_coproc_not, da_i_gate_386, copr_not_avaliable, privilege_kernel);
    init_idt_desc(int_vector_double_fault, da_i_gate_386, double_fault, privilege_kernel);
    init_idt_desc(int_vector_coproc_seg, da_i_gate_386, copr_seg_overrun, privilege_kernel);
    init_idt_desc(int_vector_inval_tss, da_i_gate_386, inval_tss, privilege_kernel);
    init_idt_desc(int_vector_seg_not, da_i_gate_386, segment_not_present, privilege_kernel);
    init_idt_desc(int_vector_stack_fault, da_i_gate_386, stack_exception, privilege_kernel);
    init_idt_desc(int_vector_protection, da_i_gate_386, general_protection, privilege_kernel);
    init_idt_desc(int_vector_page_fault, da_i_gate_386, page_fault, privilege_kernel);
    init_idt_desc(int_vector_coproc_err, da_i_gate_386, copr_error, privilege_kernel);

    // 为硬件中断设置相应的处理程序
    init_idt_desc(INT_VECTOR_IRQ0 + 0, da_i_gate_386, hwint00, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 1, da_i_gate_386, hwint01, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 2, da_i_gate_386, hwint02, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 3, da_i_gate_386, hwint03, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 4, da_i_gate_386, hwint04, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 5, da_i_gate_386, hwint05, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 6, da_i_gate_386, hwint06, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ0 + 7, da_i_gate_386, hwint07, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 0, da_i_gate_386, hwint08, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 1, da_i_gate_386, hwint09, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 2, da_i_gate_386, hwint10, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 3, da_i_gate_386, hwint11, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 4, da_i_gate_386, hwint12, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 5, da_i_gate_386, hwint13, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 6, da_i_gate_386, hwint14, privilege_kernel);
    init_idt_desc(INT_VECTOR_IRQ8 + 7, da_i_gate_386, hwint15, privilege_kernel);

    init_idt_desc(INT_VECTOR_SYS_CALL, da_i_gate_386, sys_call, privilege_user);

    /* 填充GDT中进程的LDT的描述符 */
    core_memcpy(&s_tss, 0, sizeof(struct tss));
    s_tss.ss0 = SELECTOR_KERNEL_DS;
    init_descriptor(&gdt[selector_tss >> 3], vir2phys(seg2phys(SELECTOR_KERNEL_DS), (u32)&s_tss), sizeof(struct tss) - 1, da_tss_386);
    s_tss.iobase = sizeof(struct tss);

    /* 填充GDT中进程的LDT的描述符 */
    u16 selector_ldt = selector_ldt_first & SA_RPL_MASK & SA_TI_MASK;
    for (int i = 0; i < NR_TASK + NR_PROC; i++) {
        init_descriptor(&gdt[selector_ldt >> 3],
                        vir2phys(seg2phys(SELECTOR_KERNEL_DS), (u32)proc_table[i].ldts),
                        LDT_SIZE * sizeof(struct descriptor) - 1, da_ldt);
        selector_ldt += 1 << 3;
    }
}
