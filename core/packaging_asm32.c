//
// Created by qijinwen on 4/29/20.
//

#include "packaging_asm32.h"

int get_display_pose() { return display_pose; }

void set_display_pose(int pose) { display_pose = pose; }

u8* get_gdt_ptr() { return gdt_ptr; }

u8* get_idt_ptr() { return idt_ptr; }

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

const PROCESS* get_process_ready() { return p_proc_ready; }

void set_process_ready(PROCESS* process) { p_proc_ready = process; }
