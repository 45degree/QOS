#include "hd.h"
#include "interrupt.h"
#include "message.h"
#include "package_iA32/packaging_iA32.h"
#include "panic.h"
#include "printk.h"
#include "clock.h"
#include "systask.h"
#include "sys_call.h"

static u8 hd_status = 0; //!< 硬盘状态
static u8 hd_buffer[SECTOR_SIZE * 2];

static struct iden_info_ascii {
        int idx;
        int len;
        char* desc;
    } iinfo[2];
static char* iden_info_ascii_desc1 = "HD SN";
static char* iden_info_ascii_desc2 = "HD Model";

static int waitfor(int mask, int val, int timeout) {
    int t = get_ticks();
    while(((get_ticks() - t) * 1000 / HZ) < timeout) {
        if((in_byte(REG_STATUS) & mask) == val)
            return 1;
    }
    return 0;
}

/**
 * @brief
 *
 * @param cmd
 */
static void hd_cmd_out(struct hd_cmd* cmd) {
    if(!waitfor(STATUS_BSY, 0, HD_TIMEOUT))
        panic("hd error");

    out_byte(REG_DEV_CTRL, 0);
    out_byte(REG_FEATURES, cmd->features);
    out_byte(REG_NSECTOR, cmd->count);
    out_byte(REG_LBA_LOW, cmd->lba_low);
    out_byte(REG_LBA_MID, cmd->lba_mid);
    out_byte(REG_LBA_HIGH, cmd->lba_high);
    out_byte(REG_DEVICE, cmd->device);
    out_byte(REG_CMD, cmd->command);
}


/**
 * @brief 打印一些磁盘信息
 *
 * @param hdinfo
 */
static void print_hd_info(u16* hdinfo) {
    char s[64];
    int i = 0;

    iinfo[0].idx = 10, iinfo[0].len = 20, iinfo[0].desc = iden_info_ascii_desc1;
    iinfo[1].idx = 27, iinfo[1].len = 40, iinfo[1].desc = iden_info_ascii_desc2;

    for(int k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
        char* p = (char*)&hdinfo[iinfo[k].idx];
        for(i = 0; i < iinfo[k].len / 2; i++) {
            s[i * 2 + 1] = *p++;
            s[i * 2] = *p++;
        }
        s[i * 2] = 0;
        printk("%s: %s\n", iinfo[k].desc, s);
    }

    int capabilities = hdinfo[49];
    printk("LBA supported: %s\n", (capabilities & 0x0200) ? "yes" : "No");

    int cmd_set_supported = hdinfo[83];
    printk("LBA48 supported: %s\n", (cmd_set_supported & 0x0400) ? "Yes" : "No");

    int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
    printk("HD size: %xMB\n", sectors * 512 / 1000000);
}

static void interrupt_wait() {
    MESSAGE message;
    send_recv(RECEIVE, INTERRUPT, &message);
}

void init_hd() {
    u8* numOfDisk = (u8*)(0x475);
    printk("the num of Disk is %x\n", *numOfDisk);

    put_irq_handler(AT_WINI_IRQ, hd_handler);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
}

void hd_handler() {
    hd_status = in_byte(REG_STATUS);
    inform_int(TASK_HD);
}

void hd_identify(int driver) {
    struct hd_cmd cmd;
    cmd.device = MAKE_DEVICE_REG(0, driver, 0);
    cmd.command = ATA_IDENTIFY;
    hd_cmd_out(&cmd);
    interrupt_wait();
    port_read(REG_DATA, hd_buffer, SECTOR_SIZE);
    print_hd_info((u16*) hd_buffer);
}
