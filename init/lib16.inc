; ===================================================
; 该文件存放为16位实模式下的汇编函数库
; ===================================================

; ===================================================
; 函数名: ReadSector
; 功能:   根据扇区号读取一扇区的软盘内容并加载到指定区域
;         ax        扇区号
;         ss, sp    指向堆栈段和堆栈栈顶
;         es:bx     数据缓冲区
;         ch        每磁道扇区数
;         cl        要读取的扇区数
;         dl        驱动号
; ---------------------------------------------------
ReadSector:
    push bp
    mov  bp, sp
    push dx            ; 通过[bp-2]访问该值

    push cx            ; 通过[bp-4]访问该值
    div ch
    inc ah
    mov cl, ah
    mov dh, al
    shr al, 1
    mov ch, al
    and dh, 1           ; cl 起始扇区号, ch 柱面号, dh磁头号
.GoOnReading:
    mov ax, word [bp-4] ; al 读取扇区数
    mov dl, byte [bp-2] ; dl 驱动器号
    mov ah, 02H
    int 13H
    jc .GoOnReading     ; 如果读取错误CF会被置1, 这时就不停的读, 直到正确为止

    pop cx
    pop dx
    pop bp
    ret