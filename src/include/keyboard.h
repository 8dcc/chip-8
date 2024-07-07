
#ifndef KEYBOARD_H_
#define KEYBOARD_H_ 1

#include <stdbool.h>

/* Store status of key in internal keyboard */
void kb_store(int key, bool held);

/* Check if a key is being held in the virtual keyboard */
bool kb_is_held(int key);

/* Print the layout of the keyboard */
void kb_print(void);

#endif /* KEYBOARD_H_ */
