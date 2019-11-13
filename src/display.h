#ifndef _DISPLAY_h
#define _DISPLAY_h

#include <stdint.h>
#include <stddef.h>

#include "display_config.h"
#include "layer.h"
#include "pixel.h"

extern rgb_t raster[PIXEL_COUNT];

dir_t dir_CW(dir_t, int);
dir_t dir_CCW(dir_t, int);
dir_t dir_opposite(dir_t);
dir_t dir_random();

rgba_t* hsv_to_rgb(hsva_t*, rgba_t*);
hsva_t* rgb_to_hsv(rgba_t*, hsva_t*);

extern void pixelRowLength_lut_init();
extern void pixelRowStart_lut_init();
extern int pixelRowStart_lut[PIXEL_ROW_COUNT];
extern int pixelRowLength_lut[PIXEL_ROW_COUNT];
#define rc_to_i(r,c) (pixelRowStart_lut[(r)]+(c))

void display_init();
void updateDisplay();
rgb_t* render();


#endif /* _DISPLAY_h */
