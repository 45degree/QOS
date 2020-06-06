#include "package_iA32/packaging_iA32.h"
#include "process.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void restart() {}

void* core_memcpy(void* pDest, void* pSrc, int iSize) { memcpy(pDest, pSrc, iSize); }

void core_memset(void* p_dst, char ch, int size) { memset(p_dst, (int)ch, size); }

void core_strcpy(char* p_dst, char* p_src) { strcpy(p_dst, p_src); }

void out_byte(u16 port, u8 value) {}

u8 in_byte(u16 port) {}

void disable_int() {}

void enable_int() {}

void hwint00() {}
void hwint01() {}
void hwint02() {}
void hwint03() {}
void hwint04() {}
void hwint05() {}
void hwint06() {}
void hwint07() {}
void hwint08() {}
void hwint09() {}
void hwint15() {}
void hwint10() {}
void hwint11() {}
void hwint12() {}
void hwint13() {}
void hwint14() {}

void divide_error() {}
void single_step_exception() {}
void nmi() {}
void breakpoint_exception() {}
void overflow() {}
void bounds_check() {}
void inval_opcode() {}
void copr_not_avaliable() {}
void double_fault() {}
void copr_seg_overrun() {}
void inval_tss() {}
void segment_not_present() {}
void stack_exception() {}
void general_protection() {}
void page_fault() {}
void copr_error() {}

void _display_str(const char* str) {
    size_t len = strlen(str);
    len *= 2;
    display_pose += len;
}

void _display_color_str(const char* str, int TextColor) {
    size_t len = strlen(str);
    len *= 2;
    display_pose += len;
}

void sys_call() {}

int get_ticks() {}