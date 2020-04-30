#include "keyboard.h"
#include "show.h"

void task_tty() {
    while (1) { keyboard_read(); }
}