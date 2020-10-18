/**
 * @file
 * @brief
 * @author  degree45
 * @date    2020/10/2
 * @version 0.0.1
 */

#ifndef QOS_MM_PAGE_H
#define QOS_MM_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct ARDS
 * @brief Address Range Descriptor Structure
 */
struct ARDS {
#ifdef CONFIG_32
    unsigned long long address;
    unsigned long long length;
    unsigned int type;
#else
    unsigned long address;
    unsigned long length;
    unsigned int type;
#endif
} __attribute__((packed));

struct Page {
    struct Zone* parent_zone_struct;
    unsigned long phy_address;
    unsigned long attribute;
    unsigned long reference_count;

    unsigned long age;
};

struct Zone {
    struct Page* pages_group;
    unsigned long pages_length;

    unsigned long zone_start_address;
    unsigned long zone_end_address;
    unsigned long zone_length;
    unsigned long attribute;

    unsigned long page_used_count;
    unsigned long page_free_count;

    unsigned long total_page_link;

    struct Global_Memory_Descriptor* GMD_struct;
};

/**
 * @struct Global_Memory_Descriptor
 * @brief
 */
struct Global_Memory_Descriptor {
    struct ARDS ards[32];
    unsigned long ards_length;

    unsigned long* bits_map;   //!< Physical address space page mapping bitmap
    unsigned long bits_size;   //!< Number of physical address space pages
    unsigned long bits_length; //!< Physical address space page mapping bitmap length

    struct Page* page_struct; //!< point to global Page array
    unsigned long page_size;   //!< the Page's counts
    unsigned long page_length; //!< the length of the Page array

    struct Zone* zone_struct; //!< point to global Zone array
    unsigned long zone_size;   //!< the Zone's counts
    unsigned long zone_length; //!< the length of the Zone array

    unsigned long start_code, end_code, end_data, end_brk;
    unsigned long end_of_struct;
};

void init_memory();

void set_global_memory_text_start(unsigned int start_position);

void set_global_memory_text_end(unsigned int end_position);

void set_global_memory_data_end(unsigned int end_position);

void set_global_memory_brk_end(unsigned int end_position);

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
