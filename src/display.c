
#include <stdbool.h>

#include "include/display.h"

static bool display[DISP_H * DISP_W];

/*----------------------------------------------------------------------------*/

void display_clear(void) {
    for (int y = 0; y < DISP_H; y++)
        for (int x = 0; x < DISP_W; x++)
            display[DISP_W * y + x] = false;
}

void display_render(void) {
    /* TODO: Render `display' array, call from SDL loop */
}
