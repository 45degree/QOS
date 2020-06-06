/**
 * @file
 * @brief
 * 该文件实现一些全局性的功能,包括定义GDT,LDT以及其相关功能,内存地址之间的转换,内存复制等。
 * @author  45degree
 * @date    2020/4/18
 * @version 0.0.1
 */
#include "global.h"
#include "clock.h"
#include "interrupt.h"
#include "package_iA32/packaging_iA32.h"
#include "show.h"

u32 seg2phys(u16 seg) {
    DESCRIPTOR* p_dest = &gdt[seg >> (u16)3];
    return (p_dest->base_high << (u8)24 | p_dest->base_mid << (u8)16 | p_dest->base_low);
};

u32 vir2phys(u32 seg_base, u32 vir) { return seg_base + vir; }

/**
 * 重新在c语言中加载gdt和idt, 并初始化8259A
 * @note 在初始化8259A中一并设置了外部中断处理函数, 但并没有使中断生效。
 * @see init_8259A init_port
 */
void global_init() {

    //设置显示位置为0
    set_display_pose(0);

    // 复制gdt表的内容的新的位置
    core_memcpy(gdt, get_gdt_addr(), get_gdt_offset() + 1);

    // 重新设置gdt
    u32 gdt_addr = (u32)&gdt;  //获取新gdt表的地址
    set_gdt_ptr((const u32*)gdt_addr, GTD_SIZE * sizeof(DESCRIPTOR) - 1);

    // 设置idt
    u32 idt_addr = (u32)&idt;
    set_idt_ptr((const u32*)idt_addr, IDT_SIZE * sizeof(GATE) - 1);

    display_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n -----\"cstart\" begins-----\n");
    init_port();
    display_str("-----\"cstart\" ends-----\n");
}

void init_descriptor(DESCRIPTOR* p_desc, u32 base, u32 limit, enum descriptor_attribute attribute) {
    p_desc->limit_low = limit & (u32)0x0FFFF;
    p_desc->base_low = base & (u32)0x0FFFF;
    p_desc->base_mid = (base >> (u32)16) & (u32)0x0FF;
    p_desc->attr1 = attribute & (u32)0xFF;
    p_desc->limit_high_attr2 = (((limit >> (u32)16) & (u32)0x0F) | (attribute >> (u32)8)) & (u32)0xF0;
    p_desc->base_high = (base >> (u32)24) & (u32)0x0FF;
}

void __stack_chk_fail_local() { display_str("stack chk fail"); }
