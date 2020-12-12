/**
 * @file
 * @brief
 * @author  degree45
 * @date    2020/10/2
 * @version 0.0.1
 */

#ifndef _QOS_MM_PAGE_H_
#define _QOS_MM_PAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "type.h"

/**
 * @brief Address Range Descriptor Structure
 */
struct ARDS {
    u64 address; // 8 bytes
    u64 length;  // 8 bytes
    u32 type;    // 4 byte
} __attribute__((packed));

/**
 * @brief
 */
struct Page {
    struct Zone* parent_zone_struct;
    addr_t phy_address;
    op_t attribute;
    op_t reference_count;
    op_t age;
};

struct Zone {
    struct Page* pages_group;
    op_t pages_length;

    addr_t zone_start_address;
    addr_t zone_end_address;
    op_t zone_length;
    op_t attribute;

    op_t page_used_count;
    op_t page_free_count;
    op_t total_page_link;

    struct Global_Memory_Descriptor* GMD_struct;
};

/**
 * @struct Global_Memory_Descriptor
 * @brief
 */
struct Global_Memory_Descriptor {
    struct ARDS ards[32];
    unsigned long ards_length;

    op_t* bitmap;              //!< Physical address space page mapping bitmap
    unsigned long bits_size;   //!< Number of physical address space pages
    unsigned long bits_length; //!< Physical address space page mapping bitmap length

    struct Page* page_struct;  //!< point to global Page array
    unsigned long page_size;   //!< the Page's counts
    unsigned long page_length; //!< the length of the Page array

    struct Zone* zone_struct;  //!< point to global Zone array
    unsigned long zone_size;   //!< the Zone's counts
    unsigned long zone_length; //!< the length of the Zone array

    addr_t start_code, end_code, end_data, end_brk;
    unsigned long end_of_struct;
};

void init_memory();

void set_global_memory_text_start(addr_t start_position);

void set_global_memory_text_end(addr_t end_position);

void set_global_memory_data_end(addr_t end_position);

void set_global_memory_brk_end(addr_t end_position);

#define ALIGN_4BYTE_UP(addr) DIV_ROUND_UP(addr, 4)

#define ALIGN_4K_UP(addr) DIV_ROUND_UP(addr, 1UL << 12)

#define ALIGN_4K_DOWN(addr) (((addr) >> (1UL << 12)) << (1UL << 12))

#define GET_PAGE_COUNT(nr) (nr >> 12)

/**
 * @brief  running on <RING 0> \n
 *         align the memory address on the upper boundary of 4KB
 * @param  addr the memory address
 * @return aligned memory address
 */
static inline unsigned long address_4k_align_up(unsigned long addr) {
    unsigned int page_4k_size = 1U << 12;
    return (addr + page_4k_size - 1) & (~(page_4k_size - 1));
}

/**
 * @brief  running on <RING 0> \n
 *         align the memory address on the lower boundary of 4KB
 * @param  addr the memory address
 * @return aligned memory address
 */
static inline unsigned int address_4k_align_down(unsigned int addr) {
    unsigned int page_4k_size = 1U << 12;
    return (addr >> page_4k_size) << page_4k_size;
}

extern struct Global_Memory_Descriptor memory_management_struct;

extern const struct ARDS* memARDSbuf;
extern const int memARDScount;

#ifdef __cplusplus
}
#endif

#endif /* QOS_MM_PAGE_H */
