%include "sconst.inc"

_NR_get_ticks equ 0
INT_VECTOR_SYS_CALL equ 0x90

extern save
extern sys_call_table

global get_ticks
global sys_call

bits 32
[section .text]
get_ticks:
    mov eax, _NR_get_ticks
    int INT_VECTOR_SYS_CALL
    ret

sys_call:
    call save
    sti
    call [sys_call_table + eax * 4]
    mov  [esi + EAXREG - P_STACKBASE], eax
    cli
    ret