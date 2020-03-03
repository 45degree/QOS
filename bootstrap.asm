org 0x7c00

LOAD_ADDR equ 8000H

jmp entry

db 0x90
db "OSKERNEL"       ; 启动扇区名称（8字节）
dw 512              ; 每个扇区（sector）大小（必须512字节）
db 1                ; 簇（cluster）大小（必须为1个扇区）
dw 1                ; FAT起始位置（一般为第一个扇区）
db 2                ; FAT个数（必须为2）
dw 224              ; 根目录大小（一般为224项）
dw 2880             ; 该磁盘大小（必须为2880扇区1440*1024/512）
db 0xf0             ; 磁盘类型（必须为0xf0）
dw 9                ; FAT的长度（必??9扇区）
dw 18               ; 一个磁道（track）有几个扇区（必须为18）
dw 2                ; 磁头数（必??2）
dd 0                ; 不使用分区，必须是0
dd 2880             ; 重写一次磁盘大小
db 0,0,0x29         ; 意义不明（固定）
dd 0xffffffff       ; （可能是）卷标号码
db "MYFIRSTOS  "    ; 磁盘的名称（必须为11字?，不足填空格）
db "FAT12   "       ; 磁盘格式名称（必??8字?，不足填空格）
resb 18             ; 先空出18字节

entry:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov si, ax

; 清屏
clear:
    mov ax, 0x0600
    mov bx, 0x0700  ;黑底
    mov cx, 0x0000
    mov dx, 0xffff
    int 0x10

; 将内核代码加载到LOAD_ADDR处
readFloppy:
    mov ch, 1
    mov dh, 0
    mov cl, 2

    mov bx, LOAD_ADDR

    mov ah, 0x02
    mov al, 5          ; 读写的扇区数
    mov dl, 0

    INT 0x13

    jc fin

    jmp LOAD_ADDR      ; 跳转到内核地址， 跳转之后cs，ds寄存器不变，都为0

fin:
    HLT
    jmp fin

times 510-($-$$) db 0

db 0x55
db 0xaa
