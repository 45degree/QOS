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
#include "packaging_asm32.h"
#include "show.h"

system_call sys_call_table[] = {sys_get_ticks};

u32 seg2phys(u16 seg) {
    DESCRIPTOR* p_dest = &gdt[seg >> 3];
    return (p_dest->base_high << 24 | p_dest->base_mid << 16 | p_dest->base_low);
}

u32 vir2phys(u32 seg_base, u32 vir) { return seg_base + vir; }

/**
 * 重新在c语言中加载gdt和idt, 并初始化8259A
 * @note 在初始化8259A中一并设置了外部中断处理函数, 但并没有使中断生效。
 * @see init_8259A init_prot
 */
void global_init() {
    display_pose = 0;
    core_memcpy(gdt, (void*)(*((u32*)(&gdt_ptr[2]))), *((u16*)(&gdt_ptr[0])) + 1);
    u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
    u32* p_gdt_base = (u32*)(&gdt_ptr[2]);
    *p_gdt_limit = GTD_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;

    u16* p_idt_limit = (u16*)(&idt_ptr[0]);
    u32* p_idt_base = (u32*)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32)&idt;

    display_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n -----\"cstart\" begins-----\n");
    init_prot();
    display_str("-----\"cstart\" ends-----\n");
}

void init_descriptor(DESCRIPTOR* p_desc, u32 base, u32 limit, enum descriptor_attribute attribute) {
    p_desc->limit_low = limit & 0x0FFFF;
    p_desc->base_low = base & 0x0FFFF;
    p_desc->base_mid = (base >> 16) & 0x0FF;
    p_desc->attr1 = attribute & 0xFF;
    p_desc->limit_high_attr2 = (((limit >> 16) & 0x0F) | (attribute >> 8)) & 0xF0;
    p_desc->base_high = (base >> 24) & 0x0FF;
}

int sys_get_ticks() { return ticks; }

void __stack_chk_fail_local() { display_str("stack chk fail"); }
