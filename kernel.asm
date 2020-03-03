%include "pm.inc"

; 加载到内存0x8000处， 确保cs寄存器为0
org   0x8000

jmp LABEL_BEGIN

[section .gdt]
;                             段地址   段界限            属性
LABEL_GDT:         Descriptor 0,       0,                0
LABEL_DESC_CODE32: Descriptor 0,       SegCode32Len-1,   DA_C+DA_32
LABEL_DESC_VIDEO:  Descriptor 0B8000H, 0FFFFH,           DA_DRW
LABEL_DESC_STACK:  Descriptor 0,       TopOfStack,       DA_DRWA + DA_32
LABEL_DESC_DATA:   Descriptor 0,       DATA_LEN - 1,     DA_DRW
LABEL_DESC_LDT:    Descriptor 0,       LDT_LEN - 1,      DA_LDT

LABEL_DESC_CODE_DEST: Descriptor 0,    SegCodeDestLen - 1, DA_C + DA_32
LABEL_CALL_GATE_TEST: Gate    SelectorCodeDest,  0,  0,  DA_386CGate + DA_DPL0


GdtLen equ $-LABEL_GDT
GdtPtr dw  GdtLen
       dd  0

; GDT选择子
SelectorCode32 equ LABEL_DESC_CODE32 - LABEL_GDT
SelectorVideo  equ LABEL_DESC_VIDEO  - LABEL_GDT
SelectorStack  equ LABEL_DESC_STACK  - LABEL_GDT
SelectorDate   equ LABEL_DESC_DATA   - LABEL_GDT
SelectorLDT    equ LABEL_DESC_LDT    - LABEL_GDT

SelectorCodeDest equ LABEL_DESC_CODE_DEST - LABEL_GDT
SelectorCallGateTest equ LABEL_CALL_GATE_TEST - LABEL_GDT

[section .data1]
align 32
[bits 32]
LABEL_DATA:
SPValueInRealMode	dw	0
; 字符串
MESSAGE: db "test in protect", 0
OffSetMessage equ MESSAGE - $$      ; $$ 表示当前段的起始位置， 即LABEL_DATA
DATA_LEN equ $ - LABEL_DATA


; 定义栈，栈大小512字节
[section .gs]
align 32
[bits 32]
LABEL_STACK:
    times 512 db 0
TopOfStack equ $ - LABEL_STACK - 1  ; 栈的段界限


[section .s16]
[bits 16]
LABEL_BEGIN:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 8000H

    ; 初始化LABEL_DESC_CODE32段在GDT中的段起始地址
    xor eax, eax
    mov ax,  cs
    shl eax, 4
    add eax, LABEL_SEG_CODE32
    mov word [LABEL_DESC_CODE32 + 2], ax
    shr eax, 16
    mov byte [LABEL_DESC_CODE32 + 4], al
    mov byte [LABEL_DESC_CODE32 + 7], ah

    ; 初始化LABEL_STACK段在GDT中的段起始地址
    xor eax, eax
    mov ax,  ds
    shl eax, 4
    add eax, LABEL_STACK
    mov word [LABEL_DESC_STACK + 2], ax
    shr eax, 16
    mov byte [LABEL_DESC_STACK + 4], al
    mov byte [LABEL_DESC_STACK + 7], ah

    ; 初始化LABEL_DATA段在GDT中的段起始地址
    xor eax, eax
    mov ax,  ds
    shl eax, 4
    add eax, LABEL_DATA
    mov word [LABEL_DESC_DATA + 2], ax
    shr eax, 16
    mov byte [LABEL_DESC_DATA + 4], al
    mov byte [LABEL_DESC_DATA + 7], ah

    ; 初始化ldt段在GTD中的起始位置
    xor eax, eax
    mov ax,  ds
    shl eax, 4
    add eax, LABEL_LDT
    mov word [LABEL_DESC_LDT + 2], ax
    shr eax, 16
    mov byte [LABEL_DESC_LDT + 4], al
    mov byte [LABEL_DESC_LDT + 7], ah

    xor eax, eax
    mov ax,  cs
    shl eax, 4
    add eax, LABEL_SEG_CODE_DEST
    mov word [LABEL_DESC_CODE_DEST + 2], ax
    shr eax, 16
    mov byte [LABEL_DESC_CODE_DEST + 4], al
    mov byte [LABEL_DESC_CODE_DEST + 7], ah
    ; ===============================================
    ; 以下内容用与初始化ldt中的内容
    ; =============================================== 
    
    ; 初始化LABEL_CODE_A在ldt中的位置
    xor eax, eax
    mov ax,  ds
    shl eax, 4
    add eax, LABEL_CODE_A
    mov word [LABEL_LDT_DESC_CODEA + 2], ax
    shr eax, 16
    mov byte [LABEL_LDT_DESC_CODEA + 4], al
    mov byte [LABEL_LDT_DESC_CODEA + 7], ah

    ; ===============================================
    ; 进入保护模式
    ; ===============================================
    ; 将GDT表的起始位置写如[Gdt+2]中
    xor eax, eax
    mov ax,  ds
    shl eax, 4
    add eax, LABEL_GDT
    mov dword [GdtPtr + 2], eax

    lgdt [GdtPtr]

    cli  ; 关中断

    ; 打开A20地址线
    in al, 92H
    or al, 00000010B
    out 92H, al

    ; 将cr0的最后一位置为1, 开启保护模式
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp dword SelectorCode32: 0

[section .s32]
[bits 32]
LABEL_SEG_CODE32:

    ; 设置栈段
    mov ax, SelectorStack
    mov ss, ax
    mov esp, TopOfStack

    ; 设置数据段
    mov ax, SelectorDate
    mov ds, ax

    ; 设置video段
    mov ax,  SelectorVideo
    mov gs,  ax

    ; 显示字符
    mov ah, 0CH
    xor esi, esi
    xor edi, edi
    mov esi, OffSetMessage
    mov edi, (80 * 10 + 0) * 2
    cld                            ; DF位置0, 操作方向为正向操作
.1:
    lodsb                          ; 将[ds:esi]的内容移动al内，之后esi加1(esi加1或减1与DF位有关)
    test al, al                    ; 判断al寄存器中是为0
    jz .2
    mov [gs:edi], ax
    add edi, 2
    jmp .1
.2:                                ; 显示完毕
    call DispReturn
    call SelectorCallGateTest: 0
    ; 加载LDT
    mov ax, SelectorLDT
    lldt ax
    
    ; 跳入到局部代码
    jmp SelectorLDTCodeA: 0

end:
    OffSetEnd equ $ - $$
    jmp $ 

; ================================================================
; 以下为一些封装好的调用方法
; ================================================================
; 显示AL寄存器中的值
DispAL:
    push ecx
    push ebx

    mov ah,  0CH  ; 黑底红字
    mov bl,  al
    shr al,  4    ; al中高4位移至底四位
    mov ecx, 2

    .begin:
        and al, 01111b  ; 取al底四位值
        cmp al, 9
        ja  .1
        add al, '0'
        jmp .2
    .1:                 ; 将A-F转换成‘A’-‘F’
        sub al, 10
        add al, 'A'
    .2:
        mov [gs:edi], ax
        mov al,  bl
        add edi, 2
        loop .begin

    add edi, 2
    pop ebx
    pop ecx
    ret

; 显示一个回车换行符(\r\n)
DispReturn:
    push eax
    push ebx
    mov  eax, edi
    mov  bl,  160
    div  bl
    and  eax, 0FFH
    inc  eax
    mov  bl,  160
    mul  bl
    mov  edi, eax
    pop  ebx
    pop  eax
    ret

SegCode32Len equ $-LABEL_SEG_CODE32

[section .sdest]
[bits 32]
LABEL_SEG_CODE_DEST:
    mov ax, SelectorVideo
    mov gs, ax

    mov edi, (80 * 12 + 0) * 2
    mov ah, 0CH
    mov al, 'C'

    mov [gs:edi], ax

    retf
SegCodeDestLen equ $ - LABEL_SEG_CODE_DEST

; =====================================================
; ldt代码
; =====================================================
[section .ldt]
align 32
LABEL_LDT:     
LABEL_LDT_DESC_CODEA:          Descriptor 0, CodeALen - 1, DA_C + DA_32

LDT_LEN equ $ - LABEL_LDT                 ; ldt长度

; LDT 选择子
SelectorLDTCodeA equ LABEL_LDT_DESC_CODEA - LABEL_LDT + SA_TIL

[section .la]
align 32
[bits 32]
LABEL_CODE_A:
    mov ax,   SelectorVideo
    mov gs,   ax

    mov edi,  (80 * 13 + 0) * 2
    mov ah,   0CH
    mov al,   'L'
    mov [gs:edi], ax
    
    jmp SelectorCode32: OffSetEnd

CodeALen equ $ - LABEL_CODE_A
