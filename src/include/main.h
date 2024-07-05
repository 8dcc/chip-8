
#ifndef MAIN_H_
#define MAIN_H_ 1

#include <stdint.h>
#include <SDL2/SDL.h>

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;

/*----------------------------------------------------------------------------*/

static inline void set_render_color(SDL_Renderer* rend, uint32_t col) {
    const uint8_t r = (col >> 16) & 0xFF;
    const uint8_t g = (col >> 8) & 0xFF;
    const uint8_t b = (col >> 0) & 0xFF;
    const uint8_t a = 255;
    SDL_SetRenderDrawColor(rend, r, g, b, a);
}

#endif /* MAIN_H_ */
