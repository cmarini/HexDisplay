#ifndef _DISPLAY_SK6812_h
#define _DISPLAY_SK6812_h

#include <stdint.h>
#include <stddef.h>
#include "display_config.h"

#define R_VAL_LIMIT 0xFF
#define G_VAL_LIMIT 0xFF
#define B_VAL_LIMIT 0xFF
#define RGB_VAL_LIMIT 0xFF

void displayInterface_init();
void updateDisplay();
    
#endif /* _DISPLAY_SK6812_h */
