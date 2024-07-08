
#include <stdbool.h>
#include <stdio.h>
#include "include/keyboard.h"

static EKeyboardStatus kb_status = KB_NONE;
static int last_key              = 0;

static bool key_states[16];

void kb_store(int key, bool held) {
    key_states[key] = held;

    /* If we are releasing, and we are waiting for a key, store it */
    if (kb_status == KB_WAITING && !held) {
        last_key  = key;
        kb_status = KB_HAS_KEY;
    }
}

bool kb_is_held(int key) {
    return key_states[key];
}

/*----------------------------------------------------------------------------*/

void kb_wait_for_key(void) {
    kb_status = KB_WAITING;
}

EKeyboardStatus kb_get_status(void) {
    return kb_status;
}

int kb_get_last_key(void) {
    kb_status = KB_NONE;
    return last_key;
}

/*----------------------------------------------------------------------------*/

void kb_print(void) {
    const bool* k = key_states;
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
