%include "sconst.inc"

extern gdt_ptr
extern idt_ptr
extern global_init
extern kernel_main
extern s_tss
extern p_proc_ready
extern k_reenter

global _start
global save
global restart

[section .bss]
StackSpace resb 2*1024
StackTop equ $

[section .text]
_start:
    mov  esp, StackTop
    sgdt [gdt_ptr]       ; 复制gdt寄存器的值到[gtd_ptr]
    call global_init

    ; 重新加载GDT和idt
    lgdt [gdt_ptr]
    lidt [idt_ptr]

    jmp  SELECTOR_KERNEL_CS:csinit

; 设置TSS
csinit:
    xor eax, eax
    mov ax,  SELECTOR_TSS
    ltr ax
    jmp kernel_main

; -----------------------------------------------------------------------------
; 函数名:    restart
; 描述:      用于执行p_proc_ready值向的进程
; 部分说明:  该函数在c语言中只调用一次, 即在内核启动的时候调用, 之后都是在中断
;            函数处理玩之后自动调用. 由于在中断处理的时候需要保存当前进程的寄存
;            器, 故需要在tss的esp0段保存当前进程表的顶部.
; 参考:      save, sconst.inc, proc.h
; c函数原型: void restart();
; -----------------------------------------------------------------------------
restart:
    mov  esp, [p_proc_ready]           ; esp指向新进程中存储的寄存器信息
    lldt [esp + P_LDT_SEL]             ; 重新加载新进程的ldt 
    lea  eax, [esp + P_STACKTOP]       ; 将当前进程的进程表顶部保存到tss的esp0处
    mov  dword [s_tss + TSS3_S_SP0], eax
; 恢复原寄存器的值
restart_reenter:
    dec  dword[k_reenter]
    pop  gs
    pop  fs
    pop  es
    pop  ds
    popad

    add esp, 4                         ; 跳过retaddr
    iretd                              ; eip指向下一个要运行的进程的起始处

; -----------------------------------------------------------------------------
; 函数名:    save
; 描述:      保存当前寄存器的值到当前进程表, 保存顺序参考进程表中寄存器定义的顺
;            序(proc.h或则sconst.inc), 之后切换到内核堆栈, 之后在内核堆栈中压入
;            restart函数或者restart_reenter函数用于在调用save后通过ret返回到指
;            定位置。
; 部分说明:  该函数主要用于低特权级向高特权级转移的中断函数中, 当向高特权级(这
;            里主要指内核特权)转换时, 首先会在TSS中读取相应高特权级级的堆栈信息
;            (ss段寄存器和esp寄存器), 而TSS中高特权级的esp保存的是当前运行进程
;            的esp(此时esp指向进程结构的的开始处, 参考restart), 之后会以此压入
;            发生中断时的ss, esp, eflags, 这些都是当前运行的进程的寄存器的值,
;            当调用save函数时继续压入调用时的ip地址, 此时esp指向进程表的eax处,
;            所以寄存器信息会保存在到前运行进程进程表中.
; 参考:      restart, sconst.h, proc.h
; c函数原型: 无, 该函数不再c语言中调用
; -----------------------------------------------------------------------------
save:
    pushad
    push ds
    push es
    push fs
    push gs
    ; 当前esp指向进程结构表的某个位置, 在此之后不要使用push/pop来对修改进程结构表

    mov  esi, edx  ; 保存edx的值
    mov  dx, ss
    mov  ds, dx
    mov  es, dx
    mov  edx, esi  ; 恢复edx的值

    ; esi保存进程表起始位置
    mov  esi, esp

    ; 判断是否发生了中断重入
    inc  dword [k_reenter]
    cmp  dword [k_reenter], 0
    jne  .1

    ; 切换到内核堆栈
    mov  esp, StackTop

    push restart
    jmp  [esi + RETADR - P_STACKBASE] ; 跳转到调用save处
.1:
    push restart_reenter
    jmp  [esi + RETADR - P_STACKBASE]
