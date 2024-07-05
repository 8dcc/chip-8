
#include <stdbool.h>
#include "include/display.h"
#include "include/main.h"

#define COLOR_SET   0xFFFFFF
#define COLOR_UNSET 0x000000

static bool display[DISP_H * DISP_W];

/*----------------------------------------------------------------------------*/

void display_clear(void) {
    for (int y = 0; y < DISP_H; y++)
        for (int x = 0; x < DISP_W; x++)
            display[DISP_W * y + x] = false;
}

void display_render(void) {
    for (int y = 0; y < DISP_H; y++) {
        for (int x = 0; x < DISP_W; x++) {
            const uint32_t color =
              display[DISP_W * y + x] ? COLOR_SET : COLOR_UNSET;
            set_render_color(g_renderer, color);

            SDL_Rect rect;
            rect.x = x * DISP_SCALE;
            rect.y = y * DISP_SCALE;
            rect.w = DISP_SCALE;
            rect.h = DISP_SCALE;
            SDL_RenderFillRect(g_renderer, &rect);
        }
    }
}

/*----------------------------------------------------------------------------*/

bool display_draw_sprite(int x, int y, const uint8_t* bytes, int sz) {
    bool result = false;

    /* Make sure the coordinates don't exceed the screen size */
    x %= DISP_W;
    y %= DISP_H;

    /*
     * Each bit of each byte of the sprite represents a pixel on the
     * screen. For example, this is a 3 byte sprite:
     *
     *     Hex     Binary      Pixels
     *     --------------------------
     *     0xF0    11110000    ****
     *     0x80    10000000    *
     *     0xF0    11110000    ****
     */
    for (int cur_y = 0; cur_y < sz && y + cur_y < DISP_H; cur_y++) {
        const uint8_t byte = bytes[cur_y];

        for (int cur_x = 0, i = 7; i >= 0 && x + cur_x < DISP_W; cur_x++, i--) {
            const bool bit_set = (byte >> i) & 1;

            /* This function returns true if a pixel on the screen is changed
             * from set (true) to unset (false). Since we are XOR'ing, this
             * means that both were true before the change. */
            if (display[DISP_W * (y + cur_y) + (x + cur_x)] && bit_set)
                result = true;

            display[DISP_W * (y + cur_y) + (x + cur_x)] ^= bit_set;
        }
    }

    return result;
}
