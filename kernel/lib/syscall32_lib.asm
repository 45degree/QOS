%include "sconst.inc"

_NR_write equ 0
_NR_sendrec equ 1
_NR_printx equ 2

INT_VECTOR_SYS_CALL equ 0x90

extern save
extern sys_call_table
extern p_proc_ready

global write
global sendrec
global printx
global sys_call

bits 32
[section .text]
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

printx:
    mov eax, _NR_printx
    mov edx, [esp + 4]
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
