//
// Created by qijinwen on 4/29/20.
//

#include "package_iA32/packaging_iA32.h"

int display_pose = 0;

PROCESS* p_proc_ready = 0; //!< 指向下一个要运行的进程
u32 k_reenter = 0;         //!< 中断重入判断标志

u8 gdt_ptr[6];
u8 idt_ptr[6];
TSS tss; //!< tss

/**
 * @brief 从gdt_ptr或idt_ptr中获取gdt表或idt表的地址
 * @details 从gdt_ptr或idt_ptr的高4字节获取地址
 * @param ptr gdt_ptr或idt_ptr
 * @return gdt或idt的地址
 * @author 45degree
 * @since 0.0.1
 */
static u32* get_ptr_addr(u8* ptr) {
    u32* p = (u32*)&ptr[2];
    return (u32*)(*p);
}

/**
 * @brief 从gdt_ptr或idt_ptr中获取gdt表或idt表的最大偏移
 * @details 从gdt_ptr或idt_ptr的低2字节获取地址
 * @param ptr gdt_ptr或idt_ptr
 * @return gdt表或idt表的最大偏移
 * @author 45degree
 * @since 0.0.1
 */
static u16 get_ptr_offset(u8* ptr) {
    u16* p = (u16*)&ptr[0];
    return *p;
}

/**
 * @brief 设置gdt_ptr或idt_ptr指向的gdt或idt的地址和最大偏移
 * @param addr gdt或idt的地址
 * @param offset gdt表或idt表的最大偏移
 * @param ptr gdt_ptr或idt_ptr
 * @author 45degree
 * @since 0.0.1
 */
static void set_ptr(const u32* addr, u16 offset, u8* ptr) {
    u32* ptr_addr = (u32*)&ptr[2];
    *ptr_addr = (u32)addr;

    u16* ptr_offset = (u16*)&ptr[0];
    *ptr_offset = offset;
}

/**
 * @details 从gdt_ptr的高4字节中获取GDT的地址
 */
u32* get_gdt_addr() { return get_ptr_addr(gdt_ptr); }

/**
 * @details 从gdt_ptr的低2字节中获取GDT的最大偏移
 */
u16 get_gdt_offset() { return get_ptr_offset(gdt_ptr); }

/**
 * @details 将addr指针对应的32位长的地址写入gdt_ptr的高4字节, 将offset
 *          写入gdt_ptr的低2字节.
 */
void set_gdt_ptr(const u32* addr, u16 offset) { set_ptr(addr, offset, gdt_ptr); }

/**
 * @details 从idt_ptr的高4字节中获取IDT的地址
 */
u32* get_idt_addr() { return get_ptr_addr(idt_ptr); }

/**
 * @details 从idt_ptr的低2字节中获取IDT的最大偏移
 */
u16 get_idt_offset() { return get_ptr_offset(idt_ptr); }

/**
 * @details 将addr指针对应的32位长的地址写入idt_ptr的高4字节, 将offset
 *          写入idt_ptr的低2字节.
 */
void set_idt_ptr(const u32* addr, u16 offset) { set_ptr(addr, offset, idt_ptr); }

TSS* get_tss() { return &tss; }

void init_tss() {
    tss.esp0 = 0x00;
    tss.ss0 = 0x00;
    tss.backlink = 0x00;
    tss.esp1 = 0x00;
    tss.ss1 = 0x00;
    tss.esp2 = 0x00;
    tss.ss2 = 0x00;
    tss.cr3 = 0x00;
    tss.eip = 0x00;
    tss.flags = 0x00;
    tss.eax = 0x00;
    tss.ecx = 0x00;
    tss.edx = 0x00;
    tss.ebx = 0x00;
    tss.esp = 0x00;
    tss.ebp = 0x00;
    tss.esi = 0x00;
    tss.edi = 0x00;
    tss.es = 0x00;
    tss.cs = 0x00;
    tss.ss = 0x00;
    tss.ds = 0x00;
    tss.fs = 0x00;
    tss.gs = 0x00;
    tss.ldt = 0x00;
    tss.trap = 0x00;
    tss.iobase = 0x00;
}

int get_display_pose() { return display_pose; }

void set_display_pose(int pose) { display_pose = pose; }

PROCESS* get_process_ready() { return p_proc_ready; }

void set_process_ready(PROCESS* process) { p_proc_ready = process; }
