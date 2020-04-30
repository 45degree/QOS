; [section .data]
; display_pose dd 0    ; 光标当前位置
%include "sconst.inc"
extern display_pose

[section .text]
; 导出函数
global _display_str
global _display_color_str

; ------------------------------------------------------------------------
; 函数名:   _display_str
; 描述:     显示字符串, 字符串显示位置由display_pose决定
; c函数原型: void display_str(chat* str)
; ------------------------------------------------------------------------
_display_str:
    push ebp
    mov  ebp, esp
    push esi
    push edi
    
    mov  ah, 0FH ; 黑底白字
    mov  esi, [ebp + 8]
    mov  edi, [display_pose]
.begin:
    lodsb
    test al, al   ; 判断是否为字符串结尾
    jz   .end
    cmp  al, 0AH  ; 判断是否是回车
    jnz  .show
    push eax
    mov  eax, edi
    mov  bl, 160
    div  bl
    and  eax, 0FFh
    inc  eax
    mov  bl, 160
    mul  bl
    mov  edi, eax
    pop  eax
    jmp  .begin
.show:
    push es
    push eax
    mov  ax, SElECTOR_VIDEO
    mov  es, ax
    pop  eax
    mov  [es:edi], ax
    pop  es

    add  edi, 2
    jmp  .begin
.end:
    mov  dword [display_pose], edi

    pop  edi
    pop  esi
    mov  esp, ebp
    pop  ebp
    ret

; ------------------------------------------------------------------------
; 函数名:   _display_color_str
; 描述:     显示有颜色的字符串, 字符串显示位置由display_pose决定
; c函数原型: void _display_color_str(chat* str, u8 color)
; ------------------------------------------------------------------------
_display_color_str:
    push ebp
    mov  ebp, esp
    push esi
    push edi
    
    mov  ah,  [ebp + 12]
    mov  esi, [ebp + 8]
    mov  edi, [display_pose]
.begin:
    lodsb
    test al, al   ; 判断是否为字符串结尾
    jz   .end
    cmp  al, 0AH  ; 判断是否是回车
    jnz  .show
    push eax
    mov  eax, edi
    mov  bl, 160
    div  bl
    and  eax, 0FFh
    inc  eax
    mov  bl, 160
    mul  bl
    mov  edi, eax
    pop  eax
    jmp  .begin
.show:
    push es
    push eax
    mov  ax, SElECTOR_VIDEO
    mov  es, ax
    pop  eax
    mov  [es:edi], ax
    pop  es

    add  edi, 2
    jmp  .begin
.end:
    mov  dword [display_pose], edi

    pop  edi
    pop  esi
    mov  esp, ebp
    pop  ebp
    ret

