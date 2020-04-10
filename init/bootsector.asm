org 0x7c00

jmp short entry
nop

%include "fat12hdr.inc"

entry:
    mov ax, cs
    mov ds, ax
    mov ss, ax                    ; 栈段指向0x500-0x7BFF之间的自由内存区域
    mov sp, 0x7c00
    mov es, ax
    mov bp, ax

    ; 清屏
    mov  ax, 0600H
    mov  bx, 0700H
    mov  cx, 0
    mov  dx, 0184fH
    int  10H

    ; 软驱复位
    xor ah, ah
    xor dl, dl
    int 13H

    ; 开始在软盘根目录中寻找Loader.bin
    mov word [wSectorNo], SectorNoOfRootDirectory

; 选取读取根目录的一个扇区
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
    cmp word [wRootDirSizeForLoop], 0
    jz LABEL_NO_LOADERBIN      ; 读取了所有的扇区没有找到loader.bin文件
    dec word [wRootDirSizeForLoop]
    mov ax, BaseOfLoader
    mov es, ax
    mov bx, OffsetOfLoader
    mov ax, [wSectorNo]
    mov cl, 1
    mov ch, 18
    mov dl, 0
    call ReadSector            ; 读取[wSectorNo]扇区的内容到[es:bx]

    mov si, LoaderFileName     ; [ds:si]指向LoaderFileName代表的字符串
    mov di, OffsetOfLoader     ; [es:di]指向读取的内容
    cld
    mov dx, 10H                ; dx为一个扇区的条目总数, 一个条目的大小为20H

LABEL_SEARCH_FOR_LOADERBIN:
    cmp dx, 0
    jz  LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
    dec dx
    mov cx, 11                 ; 比较字符串的长度
LABEL_CMP_FILENAME:
    cmp cx, 0
    jz LABEL_FILENAME_FOUND
    dec cx
    lodsb                      ; [ds:si]的内容移置al, si加一
    cmp al, byte [es:di]
    jz LABEL_GO_ON
    jmp LABEL_DIFFERENT

LABEL_GO_ON:
    inc di
    jmp LABEL_CMP_FILENAME

LABEL_DIFFERENT:
    and di, 0FFE0H              ; di指向本条目开头
    add di, 20H                 ; di指向下一个条目开头
    mov si, LoaderFileName
    jmp LABEL_SEARCH_FOR_LOADERBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    add word [wSectorNo], 1
    jmp LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_LOADERBIN:
    jmp $

LABEL_FILENAME_FOUND:
    mov ax, RootDirSectors
    and di, 0FFE0H
    add di, 01AH
    mov cx, word [es:di]                  ; 保存loader.bin在Fat中的序号
    push cx
    add cx, ax
    add cx, DeltaSectorNo                 ; cx指向loader.bin的起始扇区号
    mov ax, BaseOfLoader
    mov es, ax
    mov bx, OffsetOfLoader
    mov ax, cx

LABEL_GOON_LOADING_FILE:
    mov cl, 1
    mov ch, 18
    mov dl, 0
    call ReadSector
    pop ax
    call GetFATEntry
    cmp ax, 0FFFH
    jz LABEL_FILE_LOADED
    push ax
    mov dx, RootDirSectors
    add ax, dx
    add ax, DeltaSectorNo
    add bx, [BPB_BytsPerSec]
    jmp LABEL_GOON_LOADING_FILE

LABEL_FILE_LOADED:
    jmp BaseOfLoader:OffsetOfLoader

GetFATEntry:
    push es
    push bx
    push ax

    ; 在BaseOfLoader后面留出4K空间用于存放FAT
    mov  ax, BaseOfLoader
    sub  ax, 0100H
    mov  es, ax
    pop  ax

    ; 让ax指向FATEntry在FAT中的偏移量
    mov  byte [bOdd], 0
    mov  bx, 3
    mul  bx
    mov  bx, 2
    div  bx
    cmp  dx, 0
    jz   LABEL_EVEN
    mov  byte [bOdd], 1
LABEL_EVEN:
    xor dx, dx
    mov bx, [BPB_BytsPerSec]
    div bx
    push dx
    mov bx, 0
    add ax, SectorNoOfFAT1
    mov cl, 2
    mov ch, 18
    mov dl, 0
    call ReadSector

    pop dx
    add bx, dx
    mov ax, [es:bx]
    cmp byte [bOdd], 1
    jnz LABEL_EVEN_2
    shr ax, 4
LABEL_EVEN_2:
    and ax, 0FFFH
LABEL_GET_FAT_ENRY_OK:
    pop bx
    pop es
    ret

%include "lib16.inc"

; 宏定义
BaseOfLoader            equ 09000H        ; Loader.bin被加载到的段地址
OffsetOfLoader          equ 0100H         ; Loader.bin被加载到的偏移地址

; 变量
wRootDirSizeForLoop     dw RootDirSectors ;记录还未读取的扇区数量
wSectorNo               dw 0              ;记录正在读取的扇区号
bOdd                    db 0

; 字符串
LoaderFileName          db "LOADER  BIN", 0


times 510 - ( $ - $$ ) db 0
dw 0xaa55
