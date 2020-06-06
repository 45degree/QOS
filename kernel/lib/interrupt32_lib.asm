%include "sconst.inc"
%macro hwint_master 1
    call save

    ; 禁止当前中断
    in  al, INT_M_CTLMASK
    or  al, (1 << %1)
    out INT_M_CTLMASK, al

    ; 发送EOI, 以便继续接受中断
    mov  al, EOI
    out  INT_M_CTL, al

    sti

    push %1
    call [irq_table + 4 * %1]
    pop  ecx

    cli

    ; 恢复当前中断
    in  al, INT_M_CTLMASK
    and al, ~(1 << %1)
    out INT_M_CTLMASK, al

    ; 将栈顶元素弹出赋值给ip, 如果发生了中断重入跳到.restart_reenter_v2, 没有重入跳到.restart_v2
    ret
%endmacro

[section .text]
extern save
extern spurious_irq
extern exception_handler
extern irq_table

; 中断处理
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

; 异常处理
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

global disable_int
global enable_int

; ------------------------------------------------------------------------
; 中断处理函数
; ------------------------------------------------------------------------
align 16
hwint00:
    hwint_master 0

align 16
hwint01:
    hwint_master 1

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

; 关闭中断接受
disable_int:
    cli
    ret

; 打开中断接受
enable_int:
    sti
    ret