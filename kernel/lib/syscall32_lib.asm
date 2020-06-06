%include "sconst.inc"

_NR_get_ticks equ 0
_NR_write equ 1
_NR_sendrec equ 2

INT_VECTOR_SYS_CALL equ 0x90

extern save
extern sys_call_table
extern p_proc_ready

global get_ticks
global sys_call
global write

bits 32
[section .text]
get_ticks:
    mov eax, _NR_get_ticks
    int INT_VECTOR_SYS_CALL
    ret

write:
    mov eax, _NR_write
    mov ebx, [esp + 4]
    mov ecx, [esp + 8]
    int INT_VECTOR_SYS_CALL
    ret

sendrec:
    mov eax, _NR_sendrec
    mov ebx, [esp + 4]  ; function
    mov ecx, [esp + 8]  ; src_dist
    mov edx, [esp + 12] ; p_msg
    int INT_VECTOR_SYS_CALL
    ret

sys_call:
    call save
    push dword [p_proc_ready]
    sti
    push edx
    push ecx
    push ebx
    call [sys_call_table + eax * 4]
    add esp, 4 * 4
    mov  [esi + EAXREG - P_STACKBASE], eax
    cli
    ret
