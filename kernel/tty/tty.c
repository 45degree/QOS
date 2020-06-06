#include "tty/tty.h"
#include "keyboard/keyboard.h"
#include "package_iA32/packaging_iA32.h"
#include "tty/console.h"
#include "show.h"

/**
 * @brief 初始化tty
 * @param original_addr tty对应的显存的起始地址
 * @param video_mem_limit tty对应的显存的最大偏移地址
 * @return v初始化后的tty
 */
static void init_tty(TTY* tty, u32 original_addr, u32 video_mem_limit) {
    init_console(&tty->console, original_addr, video_mem_limit);
    tty->tail = tty->head = 0;
}

u32 tty_read(TTY* tty) {
    if (tty->tail == tty->head)
        return 0;
    u32 ch = tty->in_buf[tty->head];
    int position = (tty->head + 1) % (TTY_IN_BYTE + 1);
    tty->head = position;
    tty->count--;
    return ch;
}

int tty_write(TTY* tty, u32 ch) {
    int position = (tty->tail + 1) % (TTY_IN_BYTE + 1);
    if (position == tty->head)
        return 0;
    tty->in_buf[tty->tail] = ch;
    tty->tail = position;
    tty->count++;
    return 1;
}

void change_tty(int num) {
    if (num < 0 || num >= NR_CONSOLES)
        return;
    current_tty = &tty_table[num];
}

void tty_show(TTY* tty) {
    show_console(&(tty->console));
}

void tty_put_char(TTY* tty) {
    u32 key = tty_read(tty);
    if (key == 0)
        return;
    console_output_char(&(tty->console), (char)key);
}

void task_tty() {
    u32 video_mem = (V_MEM_SIZE >> 1u) / NR_CONSOLES; //!< 每一个tty用的显存大小
    u32 original_mem = 0;                             //!< 第一个tty在显存中的起始地址

    current_tty = &tty_table[0];
    for (int i = 0; i < NR_CONSOLES; i++) {
        init_tty(&tty_table[i], original_mem, video_mem);
        original_mem += video_mem;
    }

    tty_show(current_tty);

    while (1) {
        keyboard_read(current_tty);
        tty_put_char(current_tty);
    }
}
