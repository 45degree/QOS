#include "keyboard.h"
#include "interrupt.h"
#include "keymap.h"
#include "show.h"

static KB_INPUT_BUFFER kb_buffer;

static int code_with_E0 = 0;
static int shift_l = 0;
static int shift_r = 0;
static int alt_l = 0;
static int alt_r = 0;
static int ctrl_l = 0;
static int ctrl_r = 0;
static int caps_lock = 0;
static int num_lock = 0;
static int scroll_lock = 0;
static int column;

static u8 get_byte_from_kb_buffer() {
    u8 scan_code;
    while (kb_buffer.count <= 0)
        ;
    disable_int();
    scan_code = *(kb_buffer.tail);
    kb_buffer.tail++;
    if (kb_buffer.tail == kb_buffer.buf + KB_IN_BYTES) {
        kb_buffer.tail = kb_buffer.buf;
    }

    kb_buffer.count--;
    enable_int();
    return scan_code;
}

void keyboard_handler(int irq) {
    u8 scan_code = in_byte(0x60);
    if (kb_buffer.count < KB_IN_BYTES) {
        *(kb_buffer.head) = scan_code;
        kb_buffer.head++;
        if (kb_buffer.head == kb_buffer.buf + KB_IN_BYTES) {
            kb_buffer.head = kb_buffer.buf;
        }
        kb_buffer.count++;
    }
}

void keyboard_read() {
    u8 scan_code;
    int make;

    u32 key = 0;
    u32* keyrow;

    if (kb_buffer.count > 0) {
        code_with_E0 = 0;
        scan_code = get_byte_from_kb_buffer();

        if (scan_code == 0xE1) {
            u8 pausebrk_scode[] = {0xE1, 0x1D, 0x45, 0xE1, 0x90, 0xC5};
            int is_pausebreak = 1;
            for (int i = 0; i < 6; i++) {
                if (get_byte_from_kb_buffer() != pausebrk_scode[i]) {
                    is_pausebreak = 0;
                    break;
                }
            }
            if (is_pausebreak) {
                key = PAUSEBREAK;
            }
        } else if (scan_code == 0xE0) {
            scan_code = get_byte_from_kb_buffer();

            if (scan_code == 0x2A) {
                if (get_byte_from_kb_buffer() == 0xE0) {
                    if (get_byte_from_kb_buffer() == 0xAA) {
                        key = PRINTSCREEN;
                        make = 0;
                    }
                }
            }

            if (key == 0) {
                code_with_E0 = 0;
            }
        }

        if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
            make = (scan_code & FLAG_BREAK ? 0 : 1);
            keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];

            column = 0;
            if (shift_l || shift_r)
                column = 1;
            if (code_with_E0) {
                column = 2;
                code_with_E0 = 0;
            }

            key = keyrow[column];
            switch (key) {
            case SHIFT_L:
                shift_l = make;
                break;
            case SHIFT_R:
                shift_r = make;
                break;
            case CTRL_L:
                ctrl_l = make;
                break;
            case CTRL_R:
                ctrl_r = make;
                break;
            case ALT_L:
                alt_l = make;
                break;
            case ALT_R:
                alt_r = make;
                break;
            default:
                break;
            }
            if (make) {
                key |= shift_l ? FLAG_SHIFT_L : 0;
                key |= shift_r ? FLAG_SHIFT_R : 0;
                key |= ctrl_l ? FLAG_CTRL_L : 0;
                key |= ctrl_r ? FLAG_CTRL_R : 0;
                key |= alt_l ? FLAG_ALT_L : 0;
                key |= alt_r ? FLAG_ALT_R : 0;
                in_process(key);
            }
        }
    }
}

void in_process(u32 key) {
    char output[2];

    core_memset(output, 0, 2);
    if (!(key & FLAG_EXT)) {
        output[0] = key & 0xFF;
        display_str(output);
    }
}

void init_keyboard() {
    kb_buffer.count = 0;
    kb_buffer.head = kb_buffer.tail = kb_buffer.buf;
    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}
