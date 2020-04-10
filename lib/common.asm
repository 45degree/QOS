extern exception_handler
extern spurious_irq

[section .text]
global core_memcpy
global out_byte
global in_byte
global divide_error
global single_step_exception
global nmi
global breakpoint_exception
global overflow
global bounds_check
global inval_opcode
global copr_not_avaliable
global double_fault
global copr_seg_overrun
global inval_tss
global segment_not_present
global stack_exception
global general_protection
global page_fault
global copr_error
global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

; ------------------------------------------------------------------------
; 函数名： core_memcpy
; 描述:   内存拷贝，仿 memcpy
; c原型:  void* core_memcpy(void* es:pDest, void* ds:pSrc, int iSize);
; ------------------------------------------------------------------------
core_memcpy:
    push ebp
    mov  ebp, esp

    push esi
    push edi
    push ecx

    mov  edi, [ebp + 8]
    mov  esi, [ebp + 12]
    mov  ecx, [ebp + 16]

.begin:
    cmp  ecx, 0
    jz   .end

    mov  al,  [ds:esi]
    mov  byte [es:edi], al
    inc  edi
    inc  esi

    dec  ecx
    jmp  .begin
.end:
    mov  eax, [ebp + 8]

    pop  ecx
    pop  edi
    pop  esi
    mov  esp, ebp  ;还原栈顶指针
    pop  ebp

    ret

; ------------------------------------------------------------------------
; 函数名： out_byte
; 描述:   写端口
; c原型:  void out_byte(u16 port, u8 value)
; ------------------------------------------------------------------------
out_byte:
    mov  edx, [esp + 4]
    mov  al,  [esp + 4 + 4]
    out  dx,  al
    nop  ; 一点延迟
    nop
    ret

; ------------------------------------------------------------------------
; 函数名： in_byte
; 描述:   读端口
; c原型:  u8 in_byte(u16 port)
; ------------------------------------------------------------------------
in_byte:
    mov  edx, [esp + 4]
    xor  eax, eax
    in   al,  dx
    nop
    nop
    ret

; ------------------------------------------------------------------------
; 中断处理函数
; ------------------------------------------------------------------------
align 16
hwint00:
    push 0
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint01:
    push 1
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint02:
    push 2
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint03:
    push 3
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint04:
    push 4
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint05:
    push 5
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint06:
    push 6
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint07:
    push 7
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint08:
    push 8
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint09:
    push 9
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint10:
    push 10
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint11:
    push 11
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint12:
    push 12
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint13:
    push 13
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint14:
    push 14
    call spurious_irq
    add  esp, 4
    hlt

align 16
hwint15:
    push 15
    call spurious_irq
    add  esp, 4
    hlt
divide_error:
    push 0xFFFFFFFF
    push 0
    jmp  exception
single_step_exception:
    push 0xFFFFFFFF
    push 1
    jmp  exception
nmi:
    push 0xFFFFFFFF
    push 2
    jmp  exception
breakpoint_exception:
    push 0xFFFFFFFF
    push 3
    jmp  exception
overflow:
    push 0xFFFFFFFF
    push 4
    jmp  exception
bounds_check:
    push 0xFFFFFFF
    push 5
    jmp  exception
inval_opcode:
    push 0xFFFFFFF
    push 6
    jmp  exception
copr_not_avaliable:
    push 0xFFFFFFF
    push 7
    jmp  exception
double_fault:
    push 8
    jmp  exception
copr_seg_overrun:
    push 0xFFFFFFFF
    push 9
    jmp  exception
inval_tss:
    push 10
    jmp  exception
segment_not_present:
    push 11
    jmp  exception
stack_exception:
    push 12
    jmp  exception
general_protection:
    push 13
    jmp  exception
page_fault:
    push 14
    jmp  exception
copr_error:
    push 0xFFFFFFFF
    push 16
    jmp  exception
exception:
    call exception_handler
    add  esp, 4 * 2
    hlt