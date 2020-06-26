#ifndef HD_H
#define HD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"

// #define MAKE_DEVICE_REG(lba, drv, lba_highest) ((lba << 6) | (drv << 4) | (lba_highest & 0xF) | 0xA0)
#define MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba) << 6) | \
                                                ((drv) << 4) | \
                                                (lba_highest & 0xF) | 0xA0)

/* 命令块寄存器端口宏 */
#define REG_DATA 0x1F0
#define REG_FEATURES 0x1F1
#define REG_ERROR REG_FEATURES
#define REG_NSECTOR 0x1F2
#define REG_LBA_LOW 0x1F3
#define REG_LBA_MID 0x1F4
#define REG_LBA_HIGH 0x1F5
#define REG_DEVICE 0x1F6
#define REG_STATUS 0x1F7

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

#define	HD_TIMEOUT 10000 /* in millisec */
#define	PARTITION_TABLE_OFFSET 0x1BE
#define ATA_IDENTIFY 0xEC
#define ATA_READ 0x20
#define ATA_WRITE 0x30

#define SECTOR_SIZE 512
#define SECTOR_BITS (SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT 9

struct hd_cmd {
    u8  features;
    u8  count;
    u8  lba_low;
    u8  lba_mid;
    u8  lba_high;
    u8  device;
    u8  command;
};

/**
 * @brief 初始化硬盘
 * 
 */
void init_hd();

void hd_handler();

void hd_identify(int driver);

#ifdef __cplusplus
};
#endif

#endif
