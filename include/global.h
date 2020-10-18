/**
 * @file
 * @brief
 * 该文件声明一些全局性的功能,包括定义GDT,LDT以及其相关功能,内存地址之间的转换,内存复制等。
 * @author  45degree
 * @date    2020/4/18
 * @version 0.0.1
 */

#ifndef QOS_GLOBAL_H
#define QOS_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define GTD_SIZE 128 //!< GDT描述符的数量
#define IDT_SIZE 256 //!< LDT描述符的数量

/**
 * @brief GDT和LDT描述符，描述一段内存的位置和属性
 *
 * 一个GDT描述符共有8个字节,
 * 其中0字节、1字节和6字节的低四位代表的是段界限,描述的是该描述符对应的内存段的最大偏移量;
 * 2字节、3字节、4字节、7字节代表的段基址,描述的是该描述符对应的内存段的起始地址,
 * 5字节和6字节的高4位代表的是段属性。LDT描述符结构与其类似。
 */
 struct descriptor {
    u16 limit_low;       //!< 段界限低16位
    u16 base_low;        //!< 段基址低16位
    u8 base_mid;         //!< 段基址3字节
    u8 attr1;            //!< 描述符属性
    u8 limit_high_attr2; //!< 低四位代表段界限高四位, 高4位代表属性的高四位
    u8 base_high;        //!< 段基址的高4位
};

/**
 * @brief 门描述符, 主要用于调用和特权级转移
 *
 * 一个门描述符共有8个字节,其中0字节、1字节、6字节、7字节代表目标代码段入口地址的偏移;
 * 2字节、3字节代表目标代码段的选择子; 4字节代表门参数个数,5字节代表门属性。
 */
 struct gate {
    u16 offset_low;  //!< 入口地址偏移的低16位
    u16 selector;    //!< 目标代码的选择子
    u8 dcount;       //!< 门描述符的参数个数
    u8 attr;         //!< 门描述符的属性
    u16 offset_high; //!< 入口地址偏移的高16位
};

/**
 * @brief 任务状态堆栈(Task Status Stack, tss), 用于低特权级向高特权级转换
 *
 * 当低特权级向高特权级转换时,堆栈将自动切换成TSS中保存的位置。
 * 例如当由ring3转移到ring1时,堆栈将被自动切换到由ss1和esp1指定的位置。
 */
 struct tss {
    u32 backlink; //!< 上一个任务的链接
    u32 esp0;     //!< 转移到0特权级时esp的值
    u32 ss0;      //!< 转移到0特权级时ss的值
    u32 esp1;     //!< 转移到1特权级时esp的值
    u32 ss1;      //!< 转移到1特权级时ss的值
    u32 esp2;     //!< 转移到2特权级时esp的值
    u32 ss2;      //!< 转移到2特权级时ss的值
    u32 cr3;      //!< cr3寄存器
    u32 eip;      //!< eip寄存器
    u32 flags;    //!< eflags寄存器
    u32 eax;      //!< eax寄存器
    u32 ecx;      //!< ecx寄存器
    u32 edx;      //!< edx寄存器
    u32 ebx;      //!< ebx寄存器
    u32 esp;      //!< esp寄存器
    u32 ebp;      //!< ebp寄存器
    u32 esi;      //!< esi寄存器
    u32 edi;      //!< edi寄存器
    u32 es;       //!< es寄存器
    u32 cs;       //!< cs寄存器
    u32 ss;       //!< ss寄存器
    u32 ds;       //!< ds寄存器
    u32 fs;       //!< fs寄存器
    u32 gs;       //!< gs寄存器
    u32 ldt;      //!< ldt选择子
    u16 trap;     //!< IO许可位图低16位
    u16 iobase;   //!< IO许可位图高16位
};

/**
 * @brief 特权级别
 *
 * 内核特权级为0级, 为最高级别, 任务特权级为1级, 用户特权级为3级(最低)
 */
enum privilege {
    privilege_kernel = 0, //!< 内核权限
    privilege_task = 1,   //!< 任务权限
    privilege_user = 3,   //!< 用户权限
};

/**
 * @brief 选择子
 *
 * @note loader.asm中选择子保持一致
 */
enum selector {
    selector_dummy = 0,        //!< 空描述符选择子
    selector_flat_c = 0x08,    //!< 全局代码段选择子
    selector_flat_rw = 0x10,   //!< 全局数据段选择子
    selector_video = 0x18 + 3, //!< 全局显存段选择子
    selector_tss = 0x20,       //!< 全局Tss选择子
    selector_ldt_first = 0x28, //!< 全局ldt描述符表选择子
};

#define SELECTOR_KERNEL_CS selector_flat_c  //!< 内核代码段选择子
#define SELECTOR_KERNEL_DS selector_flat_rw //!< 内核数据段选择子
#define LDT_SIZE 2                          //!< 每个LDT中描述符个数

#define INDEX_LDT_C  0
#define INDEX_LDT_RW 1

enum descriptor_attribute {
    // 描述符类型值说明
    da_32 = 0x4000,       //!< 32位段
    da_limit_4k = 0x8000, //!< 段界限粒度为4K字节
    da_DPL0 = 0x00,       //!< DPL = 0
    da_DPL1 = 0x20,       //!< DPL = 1
    da_DPL2 = 0x40,       //!< DPL = 2
    da_DPL3 = 0x60,       //!< DPL = 3

    // 存储段描述符类型值说明
    da_dr = 0x90,   //!< 存在的只读数据段类型值
    da_drw = 0x92,  //!< 存在的可读写数据段属性值
    da_drwa = 0x93, //!< 存在的已访问可读写数据段类型值
    da_c = 0x98,    //!< 存在的只执行代码段属性值
    da_cr = 0x9a,   //!< 存在的可执行可读代码段属性值
    da_cco = 0x9c,  //!< 存在的只执行一致代码段属性值
    da_ccor = 0x9e, //!< 存在的可执行可读一致代码段属性值

    // 系统段描述符类型值说明
    da_ldt = 0x82,        //!< 局部描述符表段类型值
    da_task_gate = 0x85,  //!< 任务门类型值
    da_tss_386 = 0x89,    //!< 可用 386 任务状态段类型值
    da_c_gate_386 = 0x8c, //!< 386 调用门类型值
    da_i_gate_386 = 0x8e, //!< 386 中断门类型值
    da_t_gate_386 = 0x8f  //!< 386 陷阱门类型值
};

// 选择子类型值说明
enum selector_attribute {
    sa_RPL0 = 0, //!< 设置选择子RPL为0
    sa_RPL1 = 1, //!< 设置选择子RPL为1
    sa_RPL2 = 2, //!< 设置选择子RPL为2
    sa_RPL3 = 3, //!< 设置选择子RPL为3
    sa_TIG = 0,  //!< 设置TI为0, 代表选择子在GDT中
    sa_TIL = 4   //!< 设置TI为1, 代表选择子在LDT中
};

#define SA_RPL_MASK 0xFFFCu //!< 选择子RPL掩码, 用于清空选择子的RPL
#define SA_TI_MASK 0xFFFBu //!< 选择子TI掩码, 用于清空选择子的TI

#define RPL_KRNL sa_RPL0 //!< 内核段选择子RPL为0
#define RPL_TASK sa_RPL1 //!< 任务段选择子RPL为1
#define RPL_USER sa_RPL3 //!< 用户段选择子RPL为3

#define STACK_SIZE_TESTA 0x8000
#define STACK_SIZE_TOTAL (STACK_SIZE_TESTA * (NR_TASK + NR_PROC))

/**
 * @brief  计算选择子对应的段描述符的基地址
 * @author 45degree
 * @param[in]  seg 段选择子
 * @return 段描述符对应的内存段的基地址
 * @since 0.0.1
 */
extern u32 seg2phys(u16 seg);

/**
 * @brief  将段基址和偏移地址转换成实际物理地址
 * @author 45degree
 * @param[in]  seg_base  段基址
 * @param[in]  vir       偏移地址
 * @return 段基址和偏移地址对应的物理地址
 * @since 0.0.1
 */
extern u32 vir2phys(u32 seg_base, u32 vir);

/**
 * @brief  设置描述符号
 * @author 45degree
 * @param[out]  p_desc    需要设置的描述符
 * @param[in]   base      描述符对应的内存段的基地址
 * @param[in]   limit     描述符对应的内存段的段界限
 * @param[in]   attribute 描述符对应的内存段的段属性
 * @since 0.0.1
 */
extern void init_descriptor(struct descriptor* p_desc, u32 base, u32 limit,
                            enum descriptor_attribute attribute);

/**
 * @brief  全局初始化
 * @author 45degree
 * @since 0.0.1
 */
extern void global_init();

extern void __stack_chk_fail_local();

extern struct descriptor gdt[GTD_SIZE]; //!< GDT表
extern struct gate idt[IDT_SIZE];       //!< idt表, 该表存储相应的中断门调用

extern char _text;
extern char _etext;
extern char _edata;
extern char _end;

#ifdef __cplusplus
};
#endif

#endif
