
#include <stdbool.h>
#include "include/keyboard.h"

#define KB_WAITING -1

static bool key_status[16];
static int last_pressed = KB_WAITING;

void kb_store(int key, bool held) {
    key_status[key] = held;
    last_pressed    = key;
}

bool kb_is_held(int key) {
    return key_status[key];
}

int kb_get_last_pressed(void) {
    // TODO: Good system for this
    return last_pressed;
}
