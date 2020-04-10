#include "global.h"
#include "interrupt.h"

u32 seg2phys(u16 seg) {
    DESCRIPTOR* p_dest = &gdt[seg >> 3];
    return (p_dest->base_high << 24 | p_dest->base_mid << 16 | p_dest->base_low);
}

u32 vir2phys(u32 seg_base, u32 vir) {
    return seg_base + vir;
}

void init() {
    core_memcpy(gdt, (void*)(*((u32*)(&gdt_ptr[2]))), *((u16*)(&gdt_ptr[0])) + 1);
    u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
    u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);
    *p_gdt_limit = GTD_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;

    u16* p_idt_limit = (u16*)(&idt_ptr[0]);
    u32* p_idt_base  = (u32*)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32)&idt;

    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n -----\"cstart\" begins-----\n");
    init_prot();
    disp_str("-----\"cstart\" ends-----\n");
}