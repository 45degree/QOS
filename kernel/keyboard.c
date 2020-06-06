#include "keyboard/keyboard.h"
#include "interrupt.h"
#include "keyboard/keymap.h"
#include "show.h"

static KB_INPUT_BUFFER kb_buffer;

static unsigned int code_with_E0 = 0;
static unsigned int shift_l = 0;
static unsigned int shift_r = 0;
static unsigned int alt_l = 0;
static unsigned int alt_r = 0;
static unsigned int ctrl_l = 0;
static unsigned int ctrl_r = 0;
static unsigned int caps_lock = 0;
static unsigned int num_lock = 0;
static unsigned int scroll_lock = 0;
static int column;

static u8 get_byte_from_kb_buffer() {
    u8 scan_code;
    while (kb_buffer.count <= 0) {}
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

static void keyboard_wait() {
    u8 kb_stat;
    do { kb_stat = in_byte(KB_CMD); } while (kb_stat & 0x02u);
}

static void keyboard_ack() {
    u8 kb_read;
    do { kb_read = in_byte(KB_DATE); } while (kb_read != KB_ACK);
}

static void set_led() {
    u8 led = (caps_lock << 2u) | (num_lock << 1u) | scroll_lock;
    keyboard_wait();
    out_byte(KB_DATE, LED_CODE);
    keyboard_ack();

    keyboard_wait();
    out_byte(KB_DATE, led);
    keyboard_ack();
}

static void in_process(u32 key, TTY* tty) {
    char output[2];
    core_memset(output, 0, 2);
    if (!(key & FLAG_EXT)) {
        output[0] = key & 0xFF;
        tty_write(tty, (u32)(*output));
    } else {
        int raw_code = key & MASK_ROW;
        switch (raw_code) {
        case ENTER:
            tty_write(tty, '\n');
        case BACKSPACE:
            tty_write(tty, '\b');
        case UP:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
                scroll_screen(&(tty->console), SCR_DN);
            }
            break;
        case DOWN:
            if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
                scroll_screen(&(tty->console), SCR_UP);
            }
            break;
        case F1:
        case F2:
        case F3:
        case F4:
        case F5:
        case F6:
        case F7:
        case F8:
        case F9:
        case F10:
        case F11:
        case F12:
            if (key & FLAG_ALT_L || key & FLAG_ALT_R) {
                change_tty(raw_code - F1);
                tty_show(current_tty);
            }
        default:
            break;
        }
    }
}

/**
 * @details 将从键盘接收到的扫描码写入缓冲区
 */
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

/**
 * @details 从tty缓冲区读取一个扫描码, 如果为特殊按键可能会读取多个扫描码,
 * 将读取到的扫描码解析成对应的32位长的字符(见keymap.h中的定义),
 * 之后将32位长的字符交给处理函数(in_process)处理
 * @see in_process
 */
void keyboard_read(TTY* tty) {
    u8 scan_code;
    int make;

    u32 key = 0;

    if (kb_buffer.count > 0) {
        code_with_E0 = 0;
        scan_code = get_byte_from_kb_buffer();

        if (scan_code == 0xE1) {                                         // pause按键按下
            u8 pauseBreak_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5}; // pause按键产生的扫描码
            int is_pauseBreak = 1;
            for (int i = 0; i < 6; i++) {
                if (get_byte_from_kb_buffer() != pauseBreak_code[i]) {
                    is_pauseBreak = 0;
                    break;
                }
            }
            if (is_pauseBreak) {
                key = PAUSEBREAK;
            }
        } else if (scan_code == 0xE0) {
            scan_code = get_byte_from_kb_buffer();

            //判断是否为printScreen按键按下的扫描码
            if (scan_code == 0x2A) {
                if (get_byte_from_kb_buffer() == 0xE0) {
                    if (get_byte_from_kb_buffer() == 0x37) {
                        key = PRINTSCREEN;
                        make = 1;
                    }
                }
            }

            //判断是否为printScreen按键松开的扫描码
            if (scan_code == 0xB7) {
                if (get_byte_from_kb_buffer() == 0xE0) {
                    if (get_byte_from_kb_buffer() == 0xAA) {
                        key = PRINTSCREEN;
                        make = 0;
                    }
                }
            }

            if (key == 0) {
                code_with_E0 = 1;
            }
        }

        if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
            // 判断该扫描码是由按下产生的还是由松开产生的，按下为1, 松开为0
            make = (scan_code & FLAG_BREAK ? 0 : 1);

            column = 0;
            int caps = shift_l || shift_r;
            if (caps_lock) {
                if ((keymap[scan_code & 0x7Fu][0] >= 'a') &&
                    (keymap[scan_code & 0x7Fu][0] <= 'z')) {
                    caps = !caps;
                }
            }
            if (caps) {
                column = 1;
            }
            if (code_with_E0) {
                column = 2;
            }

            key = keymap[scan_code & 0x7Fu][column];
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
            case CAPS_LOCK:
                if(make) {
                    caps_lock = !caps_lock;
                    set_led();
                }
                break;
            case NUM_LOCK:
                if(make) {
                    num_lock = !num_lock;
                    set_led();
                }
                break;
            case SCROLL_LOCK:
                if(make) {
                    scroll_lock = !scroll_lock;
                    set_led();
                }
                break;
            default:
                break;
            }

            if (make) {
                int pad = 0;

                if((key >= PAD_SLASH) && (key <= PAD_9)) {
                    pad = 1;
                    switch(key) {
                    case PAD_SLASH:
                        key = '/';
                        break;
                    case PAD_STAR:
                        key = '*';
                        break;
                    case PAD_MINUS:
                        key = '-';
                        break;
                    case PAD_PLUS:
                        key = '+';
                        break;
                    case PAD_ENTER:
                        key = ENTER;
                        break;
                    default:
                        if(num_lock && (key >= PAD_0 && key <= PAD_9)) {
                            key = key - PAD_0 + '0';
                        }
                        else if (num_lock && (key == PAD_DOT)) {
                            key = '.';
                        }
                        else{
                            switch (key) {
                            case PAD_HOME:
                                key = HOME;
                                break;
                            case PAD_END:
                                key = END;
                                break;
                            case PAD_PAGEUP:
                                key = PAGEUP;
                                break;
                            case PAD_PAGEDOWN:
                                key = PAGEDOWN;
                                break;
                            case PAD_INS:
                                key = INSERT;
                                break;
                            case PAD_UP:
                                key = UP;
                                break;
                            case PAD_DOWN:
                                key = DOWN;
                                break;
                            case PAD_LEFT:
                                key = LEFT;
                                break;
                            case PAD_RIGHT:
                                key = RIGHT;
                                break;
                            case PAD_DOT:
                                key = DELETE;
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }
                key |= shift_l ? FLAG_SHIFT_L : 0;
                key |= shift_r ? FLAG_SHIFT_R : 0;
                key |= ctrl_l ? FLAG_CTRL_L : 0;
                key |= ctrl_r ? FLAG_CTRL_R : 0;
                key |= alt_l ? FLAG_ALT_L : 0;
                key |= alt_r ? FLAG_ALT_R : 0;
                in_process(key, tty);
            }
        }
    }
}

void init_keyboard() {
    kb_buffer.count = 0;
    kb_buffer.head = kb_buffer.tail = kb_buffer.buf;

    shift_r = shift_l = 0;
    alt_r = alt_l = 0;
    ctrl_r = ctrl_l = 0;

    caps_lock = 0;
    num_lock = 1;
    scroll_lock = 0;

    set_led();
    put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
    enable_irq(KEYBOARD_IRQ);
}
