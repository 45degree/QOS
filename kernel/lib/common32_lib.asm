global core_memcpy
global core_strcpy
global core_memset
global out_byte
global in_byte
global port_read

[section .text]
; ------------------------------------------------------------------------
; 函数名：core_memcpy
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
; 函数名：out_byte
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
; 函数名：in_byte
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
; 函数名: port_read
; 描述:   从指定端口读入n个数据
; c原型:  void port_read(u16 port, void* dest, int n);
; ------------------------------------------------------------------------
port_read:
    push ebp
    mov  ebp, esp

    push edx
    push edi
    push ecx

    mov edx, [ebp + 8];
    mov edi, [ebp + 12];
    mov ecx, [ebp + 16];

    shr ecx, 1
    cld
    rep insw

    pop ecx
    pop edi
    pop edx
    mov esp, ebp
    pop ebp
    ret

; ------------------------------------------------------------------------
; 函数名：core_strcpy
; 描述:   复制字符串
; c原型:  void core_strcpy(char* p_dst, char* p_src)
; ------------------------------------------------------------------------
core_strcpy:
    push ebp
    mov  ebp, esp

    mov  esi, [ebp + 12]
    mov  edi, [ebp + 8]
.1:
    mov  al,  [esi]
    inc  esi

    mov  byte [edi], al
    inc  edi
    cmp  al, 0
    jnz  .1

    pop  ebp
    ret

; ------------------------------------------------------------------------
; 函数名：core_memset
; 描述:   从内存指定位置开始复制字符(一个无符号字符)n次
; c原型:  void core_memset(void* p_dst, char ch, int size)
; ------------------------------------------------------------------------
core_memset:
    push ebp
    mov  ebp, esp

    push esi
    push edi
    push edx
    push ecx

    mov edi, [ebp + 8]
    mov edx, [ebp + 12]
    mov ecx, [ebp + 16]
.1:
    cmp ecx, 0
    jz .2

    mov byte [edi], dl
    inc edi

    dec ecx
    jmp .1
.2:
    pop ecx
    pop edx
    pop edi
    pop esi

    mov esp, ebp
    pop ebp
    ret
