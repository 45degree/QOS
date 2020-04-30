
; 从bootsector跳转到该文件时cs=09000H
org 0100H

; 宏定义
BaseOfLoader            equ 09000H                 ; loader.bin(当前文件)被加载到的内存段地址
OffsetOfLoader          equ 0100H                  ; loader.bin(当前文件)被加载到的内存偏移地址
BaseOfLoaderPhyAddr     equ BaseOfLoader*10H       ; loader.bin(当前文件)被加载到的内存物理地址

BaseOfStack             equ 0100H                  ; 栈顶位置
BaseOfKernelFile        equ 08000H                 ; kernel.bin被加载到的内存段地址
OffsetOfKernelFile      equ 0H                     ; kernel.bin被加载到的内存偏移地址
BaseOfKernelFilePhyAddr	equ	BaseOfKernelFile * 10h ; kernel.bin被加载到的物理内存地址
KernelEntryPointPhyAddr	equ	030400h

PageDirBase             equ 100000h	               ; 页目录开始地址: 1M
PageTblBase             equ 101000h	               ; 页表开始地址:   1M + 4K

jmp LABEL_START

%include "fat12hdr.inc"
%include "pm.inc"

; GDT
LABEL_GDT:            Descriptor 0,           0,        0 ; 空描述符
LABEL_DESC_FLAT_C:    Descriptor 0,           0fffffH,  DA_CR|DA_32|DA_LIMIT_4K
LABEL_DESC_FLAT_RW:   Descriptor 0,           0FFFFFH,  DA_DRW|DA_32|DA_LIMIT_4K
LABEL_DESC_VIDEO:     Descriptor 0B8000H,     0FFFFH,   DA_DRW|DA_DPL3

GdtLen  equ $-LABEL_GDT
GdtPtr  dw  GdtLen-1
        dd  BaseOfLoaderPhyAddr + LABEL_GDT

; GDT选择子
SelectorFlatC  equ LABEL_DESC_FLAT_C  - LABEL_GDT
SelectorFlatRW equ LABEL_DESC_FLAT_RW - LABEL_GDT
SelectorVideo  equ LABEL_DESC_VIDEO   - LABEL_GDT + SA_RPL3


LABEL_START:
    mov ax, cs
    mov ds, ax
    mov ss, ax
    mov sp, BaseOfStack

    mov dh, 0
    call DispStrRealMode

    ; 读取内存信息
    mov  ebx, 0
    mov  di,  _MemChkBuf    ; es:di指向写入ARDS缓冲区地址
MemChkLoop:
    mov  eax, 0x0000e820
    mov  ecx, 20             ; ARDS大小
    mov  edx, 0x0534d4150    ; "SMAP"
    int  15H
    jc   MemChkFail
    add  di, 20              ; 指向缓冲区下一个ARDS
    inc  dword [_ddMCRCount]
    cmp  ebx, 0
    je   MemChkFinish
    jmp  MemChkLoop
MemChkFail:
    mov dword [_ddMCRCount], 0
    mov dh, 3
    call DispStrRealMode
    jmp $

MemChkFinish:
    mov  dh, 4
    call DispStrRealMode

    ; 软驱复位
    mov word [wSectorNo], SectorNoOfRootDirectory
    xor ah, ah
    xor dl, dl
    int 13H

LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
    cmp word [wRootDirSizeForLoop], 0
    jz LABEL_NO_KERNELBIN
    dec word [wRootDirSizeForLoop]

    ; 读取根目录信息到kernel.bin文件加载位置(每次加载512字节)
    mov ax, BaseOfKernelFile
    mov es, ax
    mov bx, OffsetOfKernelFile
    mov ax, [wSectorNo]
    mov cl, 1
    mov ch, 18
    mov dl, 0
    call ReadSector

    mov si, KernelFileName
    mov di, OffsetOfKernelFile
    cld
    mov dx, 10H
LABEL_SEARCH_FOR_KERNELBIN:
    ; 读取一个section的信息
    cmp dx, 0
    jz  LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
    dec dx
    mov cx, 11
LABEL_CMP_FILENAME:
    cmp cx, 0
    jz LABEL_FILENAME_FOUND
    dec cx
    lodsb
    cmp al, byte [es:di]
    jz LABEL_GO_ON
    jmp LABEL_DIFFERENT
LABEL_GO_ON:
    inc di
    jmp LABEL_CMP_FILENAME

LABEL_DIFFERENT:
    ; 加载下一个section
    and di, 0FFE0H
    add di, 20h
    mov si, KernelFileName
    jmp LABEL_SEARCH_FOR_KERNELBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    ; 读取根目录的下一个扇区
    add word [wSectorNo], 1
    jmp LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_KERNELBIN:
    mov dh, 2
    call DispStrRealMode
    jmp $

; 开始加载kernel.bin
; TODO
; kernel.bin过大会读取失败, 由于bx的原因导致只能读取64kb
LABEL_FILENAME_FOUND:
    mov  ax, RootDirSectors
    and  di, 0FFF0H

    push eax
    mov  eax, [es:di + 01CH]
    mov  dword [dwKernelSize], eax
    pop  eax

    add  di, 01AH
    mov  cx, word [es:di]
    push cx
    add  cx, ax
    add  cx, DeltaSectorNo
    mov  ax, BaseOfKernelFile
    mov  es, ax
    mov  bx, OffsetOfKernelFile
    mov  ax, cx
LABEL_GOON_LOADING_FILE:
    mov  cl, 1
    mov  ch, 18
    mov  dl, 0
    call ReadSector
    pop  ax
    call GetFATEntry
    cmp  ax, 0FFFH
    jz   LABEL_FILE_LOADED
    push ax
    mov  dx, RootDirSectors
    add  ax, dx
    add  ax, DeltaSectorNo
    add  bx, [BPB_BytsPerSec]
    jmp  LABEL_GOON_LOADING_FILE

LABEL_FILE_LOADED:
    call killMotor

    mov  dh, 1
    call DispStrRealMode

; 开始进入保护模式
    lgdt [GdtPtr]

    cli

    in   al, 92H
    or   al, 00000010B
    out  92H, al

    mov  eax, cr0
    or   eax, 1
    mov  cr0, eax

    jmp dword SelectorFlatC:(BaseOfLoaderPhyAddr+LABEL_PM_START)


; ========================================================================
; 变量
; ========================================================================
wRootDirSizeForLoop dw  RootDirSectors ; Root Directory 占用的扇区数
wSectorNo           dw  0              ; 要读取的扇区号
bOdd                db  0              ; 奇数还是偶数
dwKernelSize        dd  0              ; KERNEL.BIN 文件大小

; ========================================================================
; 字符串
; ========================================================================
KernelFileName      db  "KERNEL  BIN", 0
LoadMessage:        db  "Loading  "
Message1            db  "Ready.   "
Message2            db  "No KERNEL"
                    db  "Mem Chk F"
                    db  "Mem Chk S"
MessageLength       equ 9

; ========================================================================
; 函数名: ReadSector
; 功能:   根据扇区号读取一扇区的软盘内容并加载到指定区域
;         ax        扇区号
;         ss, sp    指向堆栈段和堆栈栈顶
;         es:bx     数据缓冲区
;         ch        每磁道扇区数
;         cl        要读取的扇区数
;         dl        驱动号
; ------------------------------------------------------------------------
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

; ========================================================================
; 函数名: killMotor
; 描述：  关闭马达
; ------------------------------------------------------------------------
killMotor:
    push dx
    mov dx, 03F2H
    mov al, 0
    out dx, al
    pop dx
    ret

; ========================================================================
; 函数名: DispStrRealMode
; 描述：  显示一个字符串, 函数开始时 dh 中应该是字符串序号(0-based)
; ------------------------------------------------------------------------
DispStrRealMode:
    mov	ax, MessageLength
    mul	dh
    add	ax, LoadMessage
    mov	bp, ax
    mov	ax, ds
    mov	es, ax
    mov	cx, MessageLength
    mov	ax, 01301h
    mov	bx, 0007h
    mov	dl, 0
    add	dh, 3
    int	10h
    ret

; ========================================================================
; 函数名: GetFATEntry
; 描述：  找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
; ------------------------------------------------------------------------
GetFATEntry:
    push es
    push bx
    push ax

    ; 在BaseOfLoader后面留出4K空间用于存放FAT
    mov  ax, BaseOfKernelFile
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
    xor  dx, dx
    mov  bx, [BPB_BytsPerSec]
    div  bx
    push dx
    mov  bx, 0
    add  ax, SectorNoOfFAT1
    mov  cl, 2
    mov  ch, 18
    mov  dl, 0
    call ReadSector

    pop  dx
    add  bx, dx
    mov  ax, [es:bx]
    cmp  byte [bOdd], 1
    jnz  LABEL_EVEN_2
    shr  ax, 4
LABEL_EVEN_2:
    and  ax, 0FFFH
LABEL_GET_FAT_ENRY_OK:
    pop  bx
    pop  es
    ret

; 从此以后的代码在保护模式下执行 ---------------------------------
; 32 位代码段. 由实模式跳入 ------------------------------------
[section .s32]
align 32
[bits 32]
LABEL_PM_START:
    mov  ax,  SelectorFlatRW
    mov  ds,  ax
    mov  es,  ax
    mov  gs,  ax
    mov  fs,  ax
    mov  ss,  ax
    mov  esp, TopofStack

    ; mov  ax,  SelectorVideo
    ; mov  gs,  ax

    call DispMemInfo
    call SetupPaging
    call InitKernel

    jmp SelectorFlatC:KernelEntryPointPhyAddr ; 正式进入内核

; ========================================================================
; 显示内存信息
; ========================================================================
DispMemInfo:
    push esi
    push edi
    push ecx

    mov  esi, MemChkBuf
    mov  ecx, [ddMCRCount]
.loop:
    mov  edx, 5
    mov  edi, ARDS
.1:
    push dword [esi]
    call DispInt
    pop  eax
    stosd
    add  esi, 4
    dec  edx
    cmp  edx, 0
    jnz  .1
    call DispReturn
    cmp  dword [ddType], 1
    jne  .2
    mov  eax, [ddBaseAddLow]
    add  eax, [ddLengthLow]
    cmp  eax, [ddMemSize]
    jb   .2
    mov  [ddMemSize], eax
.2:
    loop .loop

    ; 显示总内存信息
    call DispReturn
    push szRamSize
    call DispStr
    add  esp, 4

    push dword [ddMemSize]
    call DispInt
    add  esp, 4

    pop ecx
    pop edi
    pop esi
    ret

; ------------------------------------------------------------------------
; 函数名:  DispAL
; 描述:    显示 AL 中的数字
; e.g.:
;         mov  al, 03H
;         call DispAL
; ------------------------------------------------------------------------
DispAL:
    push ecx
    push edx
    push edi

    mov  edi, [dwDispPos]    ; edi指向屏幕绘图位置

    mov  ah,  0Fh			 ; 0000b: 黑底    1111b: 白字
    mov  dl,  al
    shr	 al,  4
    mov	 ecx, 2
.begin:
    and	 al, 01111b
    cmp	 al, 9
    ja	 .1
    add	 al, '0'
    jmp	 .2
.1:
    sub	 al, 0Ah
    add	 al, 'A'
.2:
    ; 保存es的值
    push es
    push eax
    ; 让es指向显存缓冲区选择子
    mov  ax, SelectorVideo
    mov  es, ax
    pop  eax
    mov	 [es:edi], ax
    ; 恢复es寄存器
    pop  es
    add	 edi, 2

    mov	 al, dl
    loop .begin

    mov	 [dwDispPos], edi

    pop	edi
    pop	edx
    pop	ecx

    ret


; ------------------------------------------------------------------------
; 函数名:  DispInt
; 描述:    显示一个整形数(32位)
; e.g.:
;         mov  eax,  8000H
;         push eax
;         call DispInt
; ------------------------------------------------------------------------
DispInt:
    mov	 eax, [esp + 4]
    shr	 eax, 24
    call DispAL

    mov	 eax, [esp + 4]
    shr	 eax, 16
    call DispAL

    mov	 eax, [esp + 4]
    shr	 eax, 8
    call DispAL

    mov	 eax, [esp + 4]
    call DispAL

    mov	 ah, 07h			; 0000b: 黑底    0111b: 灰字
    mov	 al, 'h'
    push edi
    mov	 edi, [dwDispPos]
    
    push es
    push eax
    mov  ax, SelectorVideo
    mov  es, ax
    pop  eax
    mov	 [es:edi], ax
    pop  es

    add	 edi, 4
    mov	 [dwDispPos], edi
    pop	 edi

    ret

; ------------------------------------------------------------------------
; 函数名:  DispStr
; 描述:    显示一个字符串(以0作为结束符)
; e.g.:
;          ; Str:  db "12345", 0
;          mov  eax, Str  ;Str表示字符串的地址
;          push eax
;          call DispStr
; ------------------------------------------------------------------------
DispStr:
    push ebp
    mov	 ebp, esp
    push ebx
    push esi
    push edi

    mov	 esi, [ebp + 8]	; pszInfo
    mov	 edi, [dwDispPos]
    mov	 ah, 0Fh
.1:
    lodsb
    test al, al
    jz   .2         ; 字符串显示完毕
    cmp	 al, 0Ah	; 是否是回车
    jnz  .3
    push eax
    mov	 eax, edi
    mov	 bl, 160
    div	 bl
    and	 eax, 0FFh
    inc	 eax
    mov	 bl, 160
    mul	 bl
    mov	 edi, eax
    pop	 eax
    jmp	 .1
.3:
    push es
    push eax

    mov  ax, SelectorVideo
    mov  es, ax
    pop  eax
    mov	 [es:edi], ax
    pop  es
    add	 edi, 2
    jmp	 .1

.2:
    mov	 [dwDispPos], edi

    pop	 edi
    pop	 esi
    pop	 ebx
    pop	 ebp
    ret

; ------------------------------------------------------------------------
; 函数名:  SetupPaging
; 描述:    启动分页机制(会改变寄存器的值)
; e.g.:
;         call SetupPaging
; ------------------------------------------------------------------------
SetupPaging:
    xor  edx, edx
    mov  eax, [ddMemSize]
    mov  ebx, 400000H
    div  ebx
    mov  ecx, eax
    test edx, edx
    jz   .no_remainder
    inc  ecx
.no_remainder:
    push ecx

    ; 初始化页目录
    mov  ax,  SelectorFlatRW
    mov  es,  ax
    mov  edi, PageDirBase
    xor  eax, eax
    mov  eax, PageTblBase | PG_P | PG_USU | PG_RWW
.1:
    stosd
    add  eax, 4096
    loop .1

    ; 再初始化所有页表
    pop  eax
    mov  ebx, 1024
    mul  ebx
    mov  ecx, eax
    mov  edi, PageTblBase
    xor  eax, eax
    mov  eax, PG_P | PG_USU | PG_RWW
.2:
    stosd
    add  eax, 4096
    loop .2

    mov  eax, PageDirBase
    mov  cr3, eax
    mov  eax, cr0
    or   eax, 80000000H
    mov  cr0, eax
    jmp short .3
.3:
    nop
    ret

; ------------------------------------------------------------------------
; 函数名: DispReturn
; 描述:  换行
; ------------------------------------------------------------------------
DispReturn:
    push szReturn
    call DispStr			;printf("\n");
    add	 esp, 4
    ret

; ------------------------------------------------------------------------
; 内存拷贝，仿 memcpy
; ------------------------------------------------------------------------
; void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
; ------------------------------------------------------------------------
MemCpy:
    push ebp
    mov  ebp, esp

    push esi
    push edi
    push ecx

    mov  edi, [ebp + 8]     ; Destination
    mov  esi, [ebp + 12]    ; Source
    mov  ecx, [ebp + 16]	; Counter
.1:
    cmp  ecx, 0		        ; 判断计数器
    jz   .2		            ; 计数器为零时跳出

    mov  al, [ds:esi]       ; ┓
    inc	esi			        ; ┃
                            ; ┣ 逐字节移动
    mov	byte [es:edi], al	; ┃
    inc	edi			        ; ┛

    dec	ecx		            ; 计数器减一
    jmp	.1		            ; 循环
.2:
    mov	eax, [ebp + 8]	    ; 返回值

    pop	ecx
    pop	edi
    pop	esi
    mov	esp, ebp
    pop	ebp

    ret			; 函数结束，返回

; ------------------------------------------------------------------------
; 函数名: InitKernel
; 描述:  初始化内核
; ------------------------------------------------------------------------
InitKernel:
    xor  esi, esi
    mov  cx,  word [BaseOfKernelFilePhyAddr + 2CH]
    movzx ecx, cx
    mov  esi, [BaseOfKernelFilePhyAddr + 1CH]
    add  esi, BaseOfKernelFilePhyAddr
.Begin:
    mov  eax, [esi + 0]
    cmp  eax, 0
    jz   .NoAction
    push dword [esi + 010H]
    mov  eax, [esi + 04H]
    add  eax, BaseOfKernelFilePhyAddr
    push eax
    push dword [esi + 08H]
    call MemCpy
    add  esp, 12
.NoAction:
    add  esi, 020H
    dec  ecx
    jnz  .Begin
    ret

[section .data]
; =================================================================
; 16位实模式下数据段
; =================================================================
_ddMCRCount dd 0           ; 检查完成的ARDS的数量, 为0代表检查失败
_ddMemSize  dd 0           ; 内存大小
_dwDispPos  dd (80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
_ARDS:                     ; 地址范围描述符(ARDS)
    _ddBaseAddLow  dd 0    ; 基地址低32位
    _ddBaseAddHigh dd 0    ; 基地址高32位
    _ddLengthLow   dd 0    ; 内存长度(字节), 低32位
    _ddLengthHigh  dd 0    ; 内存长度(字节), 高32位
    _ddType        dd 0    ; ARDS的类型, 用于判断是否可被OS使用, 为1代表不可用
_MemChkBuf times 256 db 0  ; 存放所有的ARDS信息, 每个ARDS共20个字节

; =================================================================
; 32位保护模式下数据段
; =================================================================

; 字符串
_szRAMSize    db  "RAM size(byte):", 0
_szReturn   db 0Ah, 0

; 以下宏定义用于在保护模式下找到数据
szReturn      equ BaseOfLoaderPhyAddr + _szReturn
szRamSize     equ BaseOfLoaderPhyAddr + _szRAMSize
dwDispPos     equ BaseOfLoaderPhyAddr + _dwDispPos
ddMCRCount    equ BaseOfLoaderPhyAddr + _ddMCRCount
ddMemSize     equ BaseOfLoaderPhyAddr + _ddMemSize
ARDS          equ BaseOfLoaderPhyAddr + _ARDS
ddBaseAddLow  equ BaseOfLoaderPhyAddr + _ddBaseAddLow
ddBaseAddHigh equ BaseOfLoaderPhyAddr + _ddBaseAddHigh
ddLengthLow   equ BaseOfLoaderPhyAddr + _ddLengthLow
ddLengthHigh  equ BaseOfLoaderPhyAddr + _ddLengthHigh
ddType        equ BaseOfLoaderPhyAddr + _ddType
MemChkBuf     equ BaseOfLoaderPhyAddr + _MemChkBuf

; 定义堆栈
StackSpace: times 1024 db 0
TopofStack equ BaseOfLoaderPhyAddr + $
