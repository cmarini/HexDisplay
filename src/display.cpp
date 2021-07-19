
#include "display.h"
#include "wiring_digital.h"
#include "component/pmc.h"
#include "component/wiring.h"
#include "mymath.h"
#include <string.h>

#include <sam3xa.h>

#define LAYER_COUNT 8

namespace {
    Layer _layers[LAYER_COUNT];
    rgb_t raster[PIXEL_COUNT];
}

Layer* Display::getAvailableLayer() {
    return Layer::getBase()->moveToTop();
}

void pixelRowLength_lut_init() {
    /* Pre-calculate the pixelRowLength_lut */ 
    for (int i = 0; i < PIXEL_ROW_COUNT; i++) {
        if (i < HEX_SIDE_LENGTH) {
            pixelRowLength_lut[i] = HEX_SIDE_LENGTH + i;
        } else {
            pixelRowLength_lut[i] = (HEX_SIDE_LENGTH*3 - 2) - i;
        }
    }
}
void pixelRowStart_lut_init() {
    /* Pre-calculate the pixelRowStart_lut */
    for (int i = 0; i < PIXEL_ROW_COUNT; i++) {
        pixelRowStart_lut[i] = 0;
        for (int j = 0; j < i; j++) {
            pixelRowStart_lut[i] += pixelRowLength_lut[j];
        }
    }
}

void display_init()
{
    pixelRowLength_lut_init();
    pixelRowStart_lut_init();    
    for (int i=0; i<LAYER_COUNT; i++) {
        _layers[i].init();
    }
    displayInterface_init();
}

rgb_t* render()
{
    uint32_t now = millis();       
    Layer* l = Layer::getBase();
    rgba_t above; // Layer (above) Color
    int p;
    memset(&raster, 0, sizeof(raster));
    while (l != NULL) {
        for (p = 0; p < PIXEL_COUNT; p++) {
            l->getPixel(p)->update(now);
            
            above = l->getPixel(p)->getCurrent();
            above.r = (uint16_t)above.r * (uint16_t)above.a / (uint16_t)A_VAL_LIMIT;
            above.g = (uint16_t)above.g * (uint16_t)above.a / (uint16_t)A_VAL_LIMIT;
            above.b = (uint16_t)above.b * (uint16_t)above.a / (uint16_t)A_VAL_LIMIT;
            
            raster[p].r = (uint16_t)above.r + (raster[p].r * (A_VAL_LIMIT - above.a) / A_VAL_LIMIT);
            raster[p].g = (uint16_t)above.g + (raster[p].g * (A_VAL_LIMIT - above.a) / A_VAL_LIMIT);
            raster[p].b = (uint16_t)above.b + (raster[p].b * (A_VAL_LIMIT - above.a) / A_VAL_LIMIT);
        }
        
        l = l->getAbove();        
    }
    #if USE_BRIGHTNESS_CORRECTION
    for (p = 0; p < PIXEL_COUNT; p++) {
        raster[p].r = brightness_lut[raster[p].r];
        raster[p].g = brightness_lut[raster[p].g];
        raster[p].b = brightness_lut[raster[p].b];
    }
    #endif
    return raster;
}
   
//void updateDisplay()
//{
//    uint32_t now = millis();
//    Pixel* pixel;
//    for (register int r = 0; r < NUM_TLC_ROWS; r++) {
//        for (register int i = 0; i < COLS_PER_TLC5958; i++) {
//            for (register int j = 0; j < NUM_CHAINED_TLC5958; j++) {
//                register int c = i + (j * COLS_PER_TLC5958);
//                if (tlcToPixel_lut[r][c] > -1) {
//                    pixel = &pixels[tlcToPixel_lut[r][c]];
//                    pixel->update(now);
//                    TLC.GS[j].OUTR = pixel->getCurrentR(); 
//                    TLC.GS[j].OUTG = pixel->getCurrentG(); 
//                    TLC.GS[j].OUTB = pixel->getCurrentB();
//                }
//            }
//            TLC.WriteGS();
//        }
//    }
//}

dir_t dir_CW(dir_t d, int c)
{
    if (c < 0) {
        return dir_CCW(d, -c);
    }
    c %= 6;
    for (; c > 0; c--)
    {
        d = (dir_t)(d << 1);
        if (d > DIR_LAST) {
            d = DIR_FIRST;
        }
    }
    return d;
}
dir_t dir_CCW(dir_t d, int c)
{
    if (c < 0) {
        return dir_CW(d, -c);
    }
    c %= 6;
    for (; c > 0; c--)
    {
        d = (dir_t)(d >> 1);
        if (d < DIR_FIRST) {
            d = DIR_LAST;
        }
    }
    return d;
}
dir_t dir_opposite(dir_t d)
{
    return dir_CW(d, 3);
}
dir_t dir_random()
{
    return (dir_t)(1 << random(6));
}




