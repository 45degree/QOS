[section .data]
dis_pose dd 0    ; 光标当前位置

; 导出函数
global _disp_str
global _disp_pose_set
global _disp_color_str

[section .text]

; ------------------------------------------------------------------------
; 函数名:   _disp_str
; 描述:     显示字符串, 字符串显示位置由dis_pose决定
; c函数原型: void disp_str(chat* str)
; ------------------------------------------------------------------------
_disp_str:
    push ebp
    mov  ebp, esp
    push esi
    push edi
    
    mov  ah, 0FH ; 黑底白字
    mov  esi, [ebp + 8]
    mov  edi, [dis_pose]
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
    mov  [gs:edi], ax
    add  edi, 2
    jmp  .begin
.end:
    mov  dword [dis_pose], edi

    pop  edi
    pop  esi
    mov  esp, ebp
    pop  ebp
    ret

; ------------------------------------------------------------------------
; 函数名:   _disp_color_str
; 描述:     显示有颜色的字符串, 字符串显示位置由dis_pose决定
; c函数原型: void _disp_color_str(chat* str, u8 color)
; ------------------------------------------------------------------------
_disp_color_str:
    push ebp
    mov  ebp, esp
    push esi
    push edi
    
    mov  ah,  [ebp + 12]
    mov  esi, [ebp + 8]
    mov  edi, [dis_pose]
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
    mov  [gs:edi], ax
    add  edi, 2
    jmp  .begin
.end:
    mov  dword [dis_pose], edi

    pop  edi
    pop  esi
    mov  esp, ebp
    pop  ebp
    ret

; ------------------------------------------------------------------------
; 函数名： _dis_pose_set
; 描述:   设置字符串光标的显示位置
; c原型:  void dis_pose_set(int pose);
; ------------------------------------------------------------------------
_disp_pose_set:
    push eax
    mov  dword eax, [esp + 8]
    mov  dword [dis_pose], eax
    pop  eax
    ret