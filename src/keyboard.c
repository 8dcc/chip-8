
#include <stdbool.h>
#include <stdio.h>
#include "include/keyboard.h"

static bool key_status[16];

void kb_store(int key, bool held) {
    key_status[key] = held;
}

bool kb_is_held(int key) {
    return key_status[key];
}

void kb_print(void) {
    const bool* k = key_status;
    printf("+---+---+---+---+\n"
           "| %d | %d | %d | %d |\n"
           "+---+---+---+---+\n"
           "| %d | %d | %d | %d |\n"
           "+---+---+---+---+\n"
           "| %d | %d | %d | %d |\n"
           "+---+---+---+---+\n"
           "| %d | %d | %d | %d |\n"
           "+---+---+---+---+\n",
           k[1], k[2], k[3], k[0xC], k[4], k[5], k[6], k[0xD], k[7], k[8], k[9],
           k[0xE], k[0xA], k[0], k[0xB], k[0xF]);
}
