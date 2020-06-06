#include "tty/console.h"
#include "package_iA32/packaging_iA32.h"
#include "show.h"

static void set_cursor(CONSOLE* console) {
    disable_int();
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, (console -> cursor >> (u32)8) & (u32)0xFF);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, console -> cursor & (u32)0xFF);
    enable_int();
}

/**
 * @brief 向控制台输出可打印字符
 * @param console
 * @param ch
 */
static void out_char(CONSOLE* console, char ch) {
    u8* position = (u8*)(V_MEM_BASE + console->cursor * 2);
    *position++ = ch;
    *position++ = DEFAULT_CHAR_COLOR;
    console->cursor++;
    set_cursor(console);
}

void init_console(CONSOLE* console, u32 original_addr, u32 video_mem_limit) {
    console->original_addr = original_addr;
    console->current_start_addr = original_addr;
    console->video_mem_limit = video_mem_limit;
    console->cursor = original_addr;
}

void console_output_char(CONSOLE* console, char str) {
    switch (str) {
    case '\b':
        if(console -> cursor > console -> original_addr) {
            console->cursor--;
            out_char(console, ' ');
            console->cursor--;
            set_cursor(console);
        }
        break;
    case '\n':
        // 如果光标不再最后一行, 设置光标到下一行开始位置
        if (console->cursor < console->original_addr + console->video_mem_limit - SCREEN_WIDTH) {
            console->cursor = console->original_addr + SCREEN_WIDTH * ((console->cursor - console->original_addr) / SCREEN_WIDTH + 1) + 1;
        }
        set_cursor(console);
        break;
    default:
        if (console ->cursor < console->original_addr + console ->video_mem_limit)
            out_char(console, str);
        break;
    }

    while (console->cursor > console->current_start_addr + SCREEN_SIZE)
        scroll_screen(console, SCR_DN);
}

void scroll_screen(CONSOLE* console, int direction) {
    if (direction == SCR_UP) {
        if (console->current_start_addr > console->original_addr) {
            console->current_start_addr -= SCREEN_WIDTH;
        }
    } else if (direction == SCR_DN) {
        if (console->current_start_addr + SCREEN_SIZE <
            console->original_addr + console->video_mem_limit) {
            console->current_start_addr += SCREEN_WIDTH;
        }
    } else {
    }
    show_console(console);
}

void show_console(CONSOLE* console) {
    disable_int();
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    out_byte(CRTC_DATA_REG, (console->current_start_addr >> (u32)8) & (u32)0xFF);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    out_byte(CRTC_DATA_REG, (console->current_start_addr & (u32)0xFF));
    enable_int();
    set_cursor(console);
}
