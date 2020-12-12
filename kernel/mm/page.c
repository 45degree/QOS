#include "mm/page.h"
#include "bitmap.h"
#include "package_iA32/packaging_iA32.h"
#include "type.h"

#define PTR_PRE_PAGE 1024
#define PAGE_OFFSET ((unsigned long)0)

struct Global_Memory_Descriptor memory_management_struct;

/**
 * calculate the total memory size which can be used by OS,
 * then initialize the bitmap, Page array and Zone array.
 * bitmap, Page array and Zone array are all behind the kernel code in memory,
 * and all of them should be aligned
 */
void init_memory() {

    struct Global_Memory_Descriptor* GMD = &memory_management_struct;

    // get the memory information
    struct ARDS* p = (struct ARDS*)memARDSbuf;
    memory_management_struct.ards_length = 0;
    for (int i = 0; i < memARDScount; i++) {
        if (i >= 32)
            break;
        GMD->ards[i].address = p->address;
        GMD->ards[i].length = p->length;
        GMD->ards[i].type = p->type;
        GMD->ards_length++;
        p++;
        if (p->type > 4 || p->length == 0 || p->type < 1)
            continue;
    }

    p = &GMD->ards[GMD->ards_length - 1];
    unsigned int TotalMem = p->address + p->length - 1;

    // init bitsmap
    // bitsmap will be set at the end of the kernel
    addr_t start_address = ALIGN_4K_UP(GMD->end_brk);
    unsigned long size = GET_PAGE_COUNT(TotalMem);
    unsigned long length = BITS_TO_ARRAY_LEN(size);
    GMD->bitmap = (op_t*)start_address;
    GMD->bits_size = size;
    GMD->bits_length = length;
    core_memset(GMD->bitmap, 0xff, length);

    // init pages
    // pages will be set at the end of the bitsmap
    start_address = ALIGN_4BYTE_UP((addr_t)(GMD->bitmap + GMD->bits_length));
    size = GET_PAGE_COUNT(TotalMem);
    length = sizeof(struct Page) * size;
    GMD->page_struct = (struct Page*)start_address;
    GMD->page_size = size;
    GMD->page_length = length;
    core_memset(GMD->page_struct, 0x00, length);

    // init zone
    // zone will be set at the end of the page
    start_address = ALIGN_4BYTE_UP((addr_t)(GMD->page_struct + GMD->page_length));
    size = 0;
    length = 5 * sizeof(struct Zone); // pre allocated 5 Zones
    GMD->zone_struct = (struct Zone*)start_address;
    GMD->zone_size = size;
    GMD->zone_length = length;
    core_memset(memory_management_struct.zone_struct, 0x00, length);

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
        for (int j = 0; j < z->pages_length; j++, p++) {
            p->parent_zone_struct = z;
            p->phy_address = start + (1UL << 12) * j;
            p->attribute = 0;

            p->reference_count = 0;
            p->age = 0;

            // set bits_map
            // TODO
            *(memory_management_struct.bitmap + ((p->phy_address >> 21) >> 6)) ^=
                1UL << (p->phy_address >> 21) % 64;
        }
    }
}

void set_global_memory_text_start(addr_t start_position) {
    memory_management_struct.start_code = start_position;
    return;
}

void set_global_memory_text_end(addr_t end_position) {
    memory_management_struct.end_code = end_position;
    return;
}

void set_global_memory_data_end(addr_t end_position) {
    memory_management_struct.end_data = end_position;
    return;
}

void set_global_memory_brk_end(addr_t end_position) {
    memory_management_struct.end_brk = end_position;
    return;
}
