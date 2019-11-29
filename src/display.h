#ifndef _DISPLAY_h
#define _DISPLAY_h

#include <stdint.h>
#include <stddef.h>

#include "display_config.h"
#include "layer.h"
//#include "pixel.h"

namespace Display {
    Layer* getAvailableLayer();
}

dir_t dir_CW(dir_t, int);
dir_t dir_CCW(dir_t, int);
dir_t dir_opposite(dir_t);
dir_t dir_random();



void display_init();
void updateDisplay();
rgb_t* render();


#endif /* _DISPLAY_h */
