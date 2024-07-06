
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "include/util.h"
#include "include/main.h"
#include "include/display.h"
#include "include/emulator.h"
#include "include/keyboard.h"

/*----------------------------------------------------------------------------*/
/* Globals */

SDL_Window* g_window     = NULL;
SDL_Renderer* g_renderer = NULL;

/*----------------------------------------------------------------------------*/
/* Main function */

int main(int argc, char** argv) {
    if (argc < 2)
        die("Usage: %s <rom>\n", argv[0]);

    const char* rom_filename = argv[1];

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        die("Unable to start SDL.");

    /* Create SDL window */
    g_window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, DISP_W * DISP_SCALE,
                                DISP_H * DISP_SCALE, 0);
    if (!g_window)
        die("Error creating SDL window.");

    /* Create SDL renderer */
    g_renderer =
      SDL_CreateRenderer(g_window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer) {
        SDL_DestroyWindow(g_window);
        die("Error creating SDL renderer.");
    }

    /* Initialize the random seed for RND instruction */
    srand(time(NULL));

    /* Initialize the emulator */
    EmulatorCtx ctx;
    emulator_init(&ctx);

    /* Load the ROM file to memory */
    emulator_load_rom(&ctx, rom_filename);

    /* Initialize the display */
    display_clear();

    /* Main loop */
    bool running = true;
    while (running) {
        /* Parse SDL events */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    running = false;
                } break;

                case SDL_KEYDOWN: {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE: {
                            running = false;
                        } break;

                        /* clang-format off */
                        case SDL_SCANCODE_1: kb_store(0x1, true); break;
                        case SDL_SCANCODE_2: kb_store(0x2, true); break;
                        case SDL_SCANCODE_3: kb_store(0x3, true); break;
                        case SDL_SCANCODE_4: kb_store(0xC, true); break;
                        case SDL_SCANCODE_Q: kb_store(0x4, true); break;
                        case SDL_SCANCODE_W: kb_store(0x5, true); break;
                        case SDL_SCANCODE_E: kb_store(0x6, true); break;
                        case SDL_SCANCODE_R: kb_store(0xD, true); break;
                        case SDL_SCANCODE_A: kb_store(0x7, true); break;
                        case SDL_SCANCODE_S: kb_store(0x8, true); break;
                        case SDL_SCANCODE_D: kb_store(0x9, true); break;
                        case SDL_SCANCODE_F: kb_store(0xE, true); break;
                        case SDL_SCANCODE_Z: kb_store(0xA, true); break;
                        case SDL_SCANCODE_X: kb_store(0x0, true); break;
                        case SDL_SCANCODE_C: kb_store(0xB, true); break;
                        case SDL_SCANCODE_V: kb_store(0xF, true); break;

                        /* clang-format on */
                        default:
                            break;
                    }
                } break;

                case SDL_KEYUP: {
                    switch (event.key.keysym.scancode) {
                        /* clang-format off */
                        case SDL_SCANCODE_1: kb_store(0x1, false); break;
                        case SDL_SCANCODE_2: kb_store(0x2, false); break;
                        case SDL_SCANCODE_3: kb_store(0x3, false); break;
                        case SDL_SCANCODE_4: kb_store(0xC, false); break;
                        case SDL_SCANCODE_Q: kb_store(0x4, false); break;
                        case SDL_SCANCODE_W: kb_store(0x5, false); break;
                        case SDL_SCANCODE_E: kb_store(0x6, false); break;
                        case SDL_SCANCODE_R: kb_store(0xD, false); break;
                        case SDL_SCANCODE_A: kb_store(0x7, false); break;
                        case SDL_SCANCODE_S: kb_store(0x8, false); break;
                        case SDL_SCANCODE_D: kb_store(0x9, false); break;
                        case SDL_SCANCODE_F: kb_store(0xE, false); break;
                        case SDL_SCANCODE_Z: kb_store(0xA, false); break;
                        case SDL_SCANCODE_X: kb_store(0x0, false); break;
                        case SDL_SCANCODE_C: kb_store(0xB, false); break;
                        case SDL_SCANCODE_V: kb_store(0xF, false); break;

                        /* clang-format on */
                        default:
                            break;
                    }
                } break;

                default:
                    break;
            }
        }

        /* Clear window */
        set_render_color(g_renderer, 0x000000);
        SDL_RenderClear(g_renderer);

        /* Render and CPU frequency is the same, 60Hz */
        emulator_frame(&ctx);

        /* Render the virtual display into the SDL window */
        display_render();

        /* Send to renderer and delay depending on FPS */
        SDL_RenderPresent(g_renderer);
        SDL_Delay(1000 / FPS);
    }

    emulator_free(&ctx);

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
