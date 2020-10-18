#include "mm/page.h"
#include "package_iA32/packaging_iA32.h"

#define PTR_PRE_PAGE 1024
#define PAGE_OFFSET ((unsigned long)0)

struct Global_Memory_Descriptor memory_management_struct;

void init_memory() {
    struct ARDS* p = (struct ARDS*)memARDSbuf;
    memory_management_struct.ards_length = 0;
    for (int i = 0; i < memARDScount; i++) {
        if (i >= 32)
            break;
        memory_management_struct.ards[i].address = p->address;
        memory_management_struct.ards[i].length = p->length;
        memory_management_struct.ards[i].type = p->type;
        memory_management_struct.ards_length++;
        p++;
        if (p->type > 4 || p->length == 0 || p->type < 1)
            break;
    }

    p = &memory_management_struct.ards[memory_management_struct.ards_length - 1];
    unsigned int TotalMem = p->address + p->length - 1;

    // init bitsmap
    memory_management_struct.bits_map =
        (unsigned long*)address_4k_align_up(memory_management_struct.end_brk);
    memory_management_struct.bits_size = TotalMem >> 12;
    memory_management_struct.bits_length =
        (((TotalMem >> 12) + sizeof(long) * 8 - 1) / 8) & (~(sizeof(long) - 1));
    core_memset(memory_management_struct.bits_map, 0xff, memory_management_struct.bits_length);

    // init pages
    memory_management_struct.page_struct = (struct Page*)address_4k_align_up(
        (unsigned long)memory_management_struct.bits_map + memory_management_struct.bits_length);
    memory_management_struct.page_size = TotalMem >> 12;
    memory_management_struct.page_length =
        ((TotalMem >> 12) + sizeof(long) - 1) & (~(sizeof(long) - 1));
    core_memset(memory_management_struct.page_struct, 0x00, memory_management_struct.page_length);

    // init zone
    memory_management_struct.zone_struct = (struct Zone*)address_4k_align_up(
        (unsigned long)memory_management_struct.page_struct + memory_management_struct.page_length);
    memory_management_struct.zone_size = 0;
    memory_management_struct.zone_length =
        5 * (sizeof(struct Zone) + sizeof(long) - 1) & (~(sizeof(long) - 1));
    core_memset(memory_management_struct.zone_struct, 0x00, memory_management_struct.zone_length);

    for (int i = 0; i < memory_management_struct.ards_length; i++) {
        unsigned long start, end;
        struct Zone* z;
        struct Page* p;

        if (memory_management_struct.ards[i].type != 1)
            continue;
        start = address_4k_align_up(memory_management_struct.ards[i].address);
        end = address_4k_align_down(memory_management_struct.ards[i].address +
                                    memory_management_struct.ards_length);
        if (end <= start)
            continue;

        z = memory_management_struct.zone_struct + memory_management_struct.zone_size;
        memory_management_struct.zone_size++;

        z->zone_start_address = start;
        z->zone_end_address = end;
        z->zone_length = start - end;

        z->page_used_count = 0;
        z->page_free_count = (end - start) >> 12;
        z->total_page_link = 0;
        z->attribute = 0;
        z->GMD_struct = &memory_management_struct;

        z->pages_length = (end - start) >> 12;
        z->pages_group = (struct Page*)(memory_management_struct.page_struct + (start >> 12));

        // page init
        p = z->pages_group;
        for(int j = 0; j < z->pages_length; j++, p++) {
            p->parent_zone_struct = z;
            p->phy_address = start + (1UL << 12) * j;
            p->attribute = 0;

            p->reference_count = 0;
            p->age = 0;

            // set bits_map
            // TODO
        }
    }
}

void set_global_memory_text_start(unsigned int start_position) {
    memory_management_struct.start_code = start_position;
    return;
}

void set_global_memory_text_end(unsigned int end_position) {
    memory_management_struct.end_code = end_position;
    return;
}

void set_global_memory_data_end(unsigned int end_position) {
    memory_management_struct.end_data = end_position;
    return;
}

void set_global_memory_brk_end(unsigned int end_position) {
    memory_management_struct.end_brk = end_position;
    return;
}
