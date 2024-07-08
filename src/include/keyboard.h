
#ifndef KEYBOARD_H_
#define KEYBOARD_H_ 1

#include <stdbool.h>

typedef enum {
    KB_NONE    = 0, /* The keyboard is not waiting for anything */
    KB_WAITING = 1, /* Waiting for a key (LD Vx, K) */
    KB_HAS_KEY = 2, /* Was waiting, but received a key */
} EKeyboardStatus;

/*----------------------------------------------------------------------------*/

/* Store status of key in internal keyboard */
void kb_store(int key, bool held);

/* Check if a key is being held in the virtual keyboard */
bool kb_is_held(int key);

/* Print the layout of the keyboard */
void kb_print(void);

/* Set the keyboard status to KB_WAITING. See EKeyboardStatus enum for more
 * information. */
void kb_wait_for_key(void);

/* Get the current keyboard status. See EKeyboardStatus enum for more
 * information. */
EKeyboardStatus kb_get_status(void);

/* After waiting, the keyboard detected a key release and stored it. This
 * function returns that key. The caller must make sure that the keyboard status
 * is KB_HAS_KEY by calling `kb_get_status'. */
int kb_get_last_key(void);

#endif /* KEYBOARD_H_ */
