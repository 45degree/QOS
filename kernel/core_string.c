//
// Created by qijinwen on 5/13/20.
//

int core_strlen(char* str) {
    int len = 0;
    while (*str) {
        len++;
        str++;
    }
    return len;
}