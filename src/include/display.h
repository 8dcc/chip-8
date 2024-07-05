
#ifndef DISPLAY_H_
#define DISPLAY_H_ 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Original display size, without scaling */
#define DISP_W 64
#define DISP_H 32

/* Scaling used when rendering each pixel */
#define DISP_SCALE 10

/* Frames per second when rendering */
#define FPS 60

/*----------------------------------------------------------------------------*/

/* Clear the display to black */
void display_clear(void);

/* Render the virtual display into the SDL window */
void display_render(void);

/* Draw a sprite into the virtual display, starting at display position
 * (x,y). For more information on the sprite format, see the comment inside the
 * function itself. */
bool display_draw_sprite(int x, int y, const uint8_t* bytes, int sz);

#endif /* DISPLAY_H_ */
