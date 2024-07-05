
#ifndef DISPLAY_H_
#define DISPLAY_H_ 1

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

#endif /* DISPLAY_H_ */
