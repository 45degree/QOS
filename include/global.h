// MIT License
// 
// Copyright (c) 2020 45度的星空
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// TODO
// 该文件定义一些全局性的功能，包括定义GDT, LDT以及其相关功能, 内存地址之间的转换, 内存复制的内容
// 等。
//
// 一个GDT描述符共有8位, 其中低两位(0字节和1字节)和6字节的低四位代表的是段界限，描述的是该描述
// 符对应的内存段的最大偏移量; 2字节, 3字节, 4字节, 7字节代表的段基址, 描述的是该描述符对应的
// 内存段的起始地址, 5字节和6字节的高4位代表的是段属性。

#ifndef CORE_TYPE_H
#define CORE_TYPE_H

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

/* GDT 和 IDT 中描述符的个数 */
#define GTD_SIZE 128
#define IDT_SIZE 256

/* 存储段描述符/系统段描述符 */
typedef struct s_descriptor {
    u16 limit_low;
    u16 base_low;
    u8  base_mid;
    u8  attr1;
    u8  limit_high_attr2;
    u8  base_high;
} DESCRIPTOR;

/* 门描述符 */
typedef struct s_gate {
    u16 offset_low;
    u16 selector;
    u8  dcount;
    u8  attr;
    u16 offset_high;
} GATE;

/* tss */
typedef struct s_tss {
    u32 backlink;
    u32 esp0;
    u32 ss0;
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldt;
    u16 trap;
    u16 iobase;
}TSS;

/* 权限 */
#define	PRIVILEGE_KRNL  0
#define	PRIVILEGE_TASK  1
#define	PRIVILEGE_USER  3

/* GDT 描述符索引 */
#define	INDEX_DUMMY     0
#define	INDEX_FLAT_C    1
#define	INDEX_FLAT_RW   2
#define	INDEX_VIDEO     3
#define INDEX_TSS       4
#define INDEX_LDT_FIRST 5

/* 选择子 和loader中选择子保持一致 */
#define	SELECTOR_DUMMY         0
#define	SELECTOR_FLAT_C     0x08
#define	SELECTOR_FLAT_RW    0x10
#define	SELECTOR_VIDEO      (0x18+3)
#define SELECTOR_TSS        0x20
#define SELECTOR_LDT_FIRST  0x28

#define	SELECTOR_KERNEL_CS  SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS  SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS  SELECTOR_VIDEO

/* 每个LDT中描述符个数 */
#define LDT_SIZE 2

/* 描述符类型值说明 */
#define DA_32            0x4000  /* 32位段*/
#define DA_LIMIT_4K      0x8000	 /* 段界限粒度为4K字节*/
#define DA_DPL0          0x00    /* DPL = 0 */
#define DA_DPL1          0x20    /* DPL = 1 */
#define DA_DPL2          0x40    /* DPL = 2 */
#define DA_DPL3          0x60    /* DPL = 3 */
/* 存储段描述符类型值说明 */
#define DA_DR            0x90    /* 存在的只读数据段类型值 */
#define DA_DRW           0x92    /* 存在的可读写数据段属性值 */
#define DA_DRWA          0x93    /* 存在的已访问可读写数据段类型值 */
#define DA_C             0x98    /* 存在的只执行代码段属性值 */
#define DA_CR            0x9A    /* 存在的可执行可读代码段属性值 */
#define DA_CCO           0x9C    /* 存在的只执行一致代码段属性值 */
#define DA_CCOR          0x9E    /* 存在的可执行可读一致代码段属性值 */
/* 系统段描述符类型值说明 */
#define DA_LDT           0x82    /* 局部描述符表段类型值 */
#define DA_TaskGate      0x85    /* 任务门类型值 */
#define DA_386TSS        0x89    /* 可用 386 任务状态段类型值 */
#define DA_386CGate      0x8C    /* 386 调用门类型值 */
#define DA_386IGate      0x8E    /* 386 中断门类型值 */
#define DA_386TGate      0x8F    /* 386 陷阱门类型值 */

/* 选择子类型值说明 */
#define SA_RPL_MASK      0xFFFC
#define SA_RPL0          0
#define SA_RPL1          1
#define SA_RPL2          2
#define SA_RPL3          3

#define RPL_KRNL         SA_RPL0
#define RPL_TASK         SA_RPL1
#define RPL_USER         SA_RPL3

#define SA_TI_MASK       0xFFFB
#define SA_TIG           0
#define SA_TIL           4

#define NR_TASKS         1
#define STACK_SIZE_TESTA 0x8000
#define STACK_SIZE_TOTAL STACK_SIZE_TESTA

u8 gdt_ptr[6];            // GDTR寄存器中的值，共6个字节，低两个字节保存的是gdt的最大偏移(长度-1)，高4字节保存gdt的起始物理地址
DESCRIPTOR gdt[GTD_SIZE]; // GDT表
u8 idt_ptr[6];            // 表示方法同gdt_ptr
GATE idt[IDT_SIZE];       // idt表
TSS  tss;

/* 内存地址转换 */
extern u32 seg2phys(u16 seg);

// #define vir2phys(seg_base, vir) (u32)(((u32)seg_base) + (u32)(vir))

extern u32 vir2phy(u32 seg_base, u32 vir);

/* 内存复制 */
extern void* core_memcpy(void* pDst, void* pSrc, int iSize);

/* 用于初始化全局 */
extern void init();
#endif