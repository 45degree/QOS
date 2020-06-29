#ifndef HD_H
#define HD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

#define MAKE_DEVICE_REG(lba, drv, lba_highest)                                                     \
    (((lba) << 6) | ((drv) << 4) | (lba_highest & 0xF) | 0xA0)

/* 命令块寄存器端口*/
enum hd_reg_port {
    REG_DATA = 0x1F0,
    REG_FEATURES = 0x1F1,
    REG_ERROR = REG_FEATURES,
    REG_NSECTOR = 0x1F2,
    REG_LBA_LOW = 0x1F3,
    REG_LBA_MID = 0x1F4,
    REG_LBA_HIGH = 0x1F5,
    REG_DEVICE = 0x1F6,
    REG_STATUS = 0x1F7
};

#define STATUS_BSY 0x80
#define STATUS_DRDY 0x40
#define STATUS_DFSE 0x20
#define STATUS_DSC 0x10
#define STATUS_DRQ 0x08
#define STATUS_CORR 0x04
#define STATUS_IDX 0x02
#define STATUS_ERR 0x01

#define REG_CMD REG_STATUS
#define REG_DEV_CTRL 0x3F6
#define REG_ALT_STATUS REG_DEV_CTRL
#define REG_DRV_ADDR 0x3F7

#define HD_TIMEOUT 10000 /* in millisec */
#define PARTITION_TABLE_OFFSET 0x1BE
#define ATA_IDENTIFY 0xEC
#define ATA_READ 0x20
#define ATA_WRITE 0x30

#define SECTOR_SIZE 512
#define SECTOR_BITS (SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT 9

#define MAX_DEVICE 2         //!< 最大能够链接2块磁盘
#define NR_PART_PRE_DEVICE 4 //!< 每个磁盘能够分4个分区(主分区和扩展分区)
#define NR_SUB_PRE_PART 16   //!< 每个分区最多有16个逻辑分区

struct hd_cmd {
    u8 features;
    u8 count;
    u8 lba_low;
    u8 lba_mid;
    u8 lba_high;
    u8 device;
    u8 command;
};

/**
 * @brief 分区表结构
 */
struct part_enter {
    u8 status;         //!< 硬盘状态，80H可引导， 00H不可引导， 其他不合法
    u8 start_head;     //!< 起始磁头号
    u8 start_sector;   //!< 起始扇区号(仅用了低6未，高2位为起始柱面号8,9位)
    u8 start_cyl_low8; //!< 起始柱面号低8位
    u8 systemID;       //!< 分区类型
    u8 end_head;       //!< 结束磁头号
    u8 end_sector;     //!< 结束扇区号(仅用了低6未，高2位为起始柱面号8,9位)
    u8 end_cyl_low8;   //!< 结束柱面号低8位
    u8 start_LBA;      //!< 起始扇区的LBA
    u8 sector_num;     //!< 扇区数目
};

struct hd_info {};

/**
 * @brief 初始化硬盘
 */
void init_hd();

void hd_handler();

void hd_identify(int driver);

void read_date_from_dh(int driver, int section, char* buffer);

#ifdef __cplusplus
};
#endif

#endif
