; ==================================================
; 该文件定义Fat12文件系统
; ==================================================

; 下面是FAT12 磁盘的头
BS_OEMName      DB  "QOS     "            ; OEM String, 必须8 个字节
BPB_BytsPerSec  DW  512                   ; 每扇区字节数
BPB_SecPerClus  DB  1                     ; 每簇多少扇区
BPB_RsvdSecCnt  DW  1                     ; Boot 记录占用多少扇区
BPB_NumFATs     DB  2                     ; 共有多少FAT 表
BPB_RootEntCnt  DW  224                   ; 根目录文件数最大值
BPB_TotSec16    DW  2880                  ; 逻辑扇区总数
BPB_Media       DB  0xF0                  ; 媒体描述符
BPB_FATSz16     DW  9                     ; 每FAT扇区数
BPB_SecPerTrk   DW  18                    ; 每磁道扇区数
BPB_NumHeads    DW  2                     ; 磁头数（面数）
BPB_HiddSec     DD  0                     ; 隐藏扇区数
BPB_TotSec32    DD  0                     ; wTotalSectorCount为0时这个值记录扇区数
BS_DrvNum       DB  0                     ; 中断13 的驱动器号
BS_Reserved1    DB  0                     ; 未使用
BS_BootSig      DB  29h                   ; 扩展引导标记（29h）
BS_VolID        DD  0                     ; 卷序列号
BS_VolLab       DB  "QOS        "         ; 卷标, 必须11 个字节
BS_FileSysType  DB  "FAT12   "            ; 文件系统类型, 必须8个字节

; 相关的宏定义
FATSz                   equ 9
SectorNoOfRootDirectory equ 19            ; Root Directory 的第一个扇区号
RootDirSectors          equ 14            ; 根目录占用空间
SectorNoOfFAT1          equ 1             ; FAT1的第一个扇区号
DeltaSectorNo           equ 17