
#include "display.h"
#include "wiring_digital.h"
#include "component/pmc.h"
#include "component/wiring.h"
#include "mymath.h"
#include <string.h>

#include <sam3xa.h>


int pixelRowStart_lut[PIXEL_ROW_COUNT];
int pixelRowLength_lut[PIXEL_ROW_COUNT];

//Layer layers[1];

rgb_t raster[PIXEL_COUNT];

void pixelRowLength_lut_init() {
    for (int i = 0; i < PIXEL_ROW_COUNT; i++) {
        if (i < HEX_SIDE_LENGTH) {
            pixelRowLength_lut[i] = HEX_SIDE_LENGTH + i;
        } else {
            pixelRowLength_lut[i] = (HEX_SIDE_LENGTH*3 - 2) - i;
        }
    }
}
void pixelRowStart_lut_init() {
    for (int i = 0; i < PIXEL_ROW_COUNT; i++) {
        pixelRowStart_lut[i] = 0;
        for (int j = 0; j < i; j++) {
            pixelRowStart_lut[i] += pixelRowLength_lut[j];
        }
    }
}

void display_init()
{
    /* Pre-calculate the pixelRowLength_lut */
    for (int i = 0; i < PIXEL_ROW_COUNT; i++) {
        if (i < HEX_SIDE_LENGTH) {
            pixelRowLength_lut[i] = HEX_SIDE_LENGTH + i;
        } else {
            pixelRowLength_lut[i] = (HEX_SIDE_LENGTH*3 - 2) - i;
        }
    }
    /* Pre-calculate the pixelRowStart_lut */
    for (int i = 0; i < PIXEL_ROW_COUNT; i++) {
        pixelRowStart_lut[i] = 0;
        for (int j = 0; j < i; j++) {
            pixelRowStart_lut[i] += pixelRowLength_lut[j];
        }
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


rgba_t* hsv_to_rgb(hsva_t* p_hsv, rgba_t* p_rgb)
{
    p_rgb->a = p_hsv->a;
    // Determine the Hue
    while (p_hsv->h >= H_VAL_LIMIT) {
        p_hsv->h -= H_VAL_LIMIT;
    }
    
    //if (p_hsv->h >= 0 && p_hsv->h < H_VAL_SECTION) {
    if (p_hsv->h < H_VAL_SECTION) { // unnecessary comparison (p_hsv->h >= 0)
        p_rgb->r = R_VAL_LIMIT;
        p_rgb->g = map(p_hsv->h, H_VAL_SECTION*0, H_VAL_SECTION*1, 0, G_VAL_LIMIT);
        p_rgb->b = 0;
    } else if (p_hsv->h < H_VAL_SECTION*2) {
        p_rgb->r = map(p_hsv->h, H_VAL_SECTION*1, H_VAL_SECTION*2, R_VAL_LIMIT, 0);
        p_rgb->g = G_VAL_LIMIT;
        p_rgb->b = 0;
    } else if (p_hsv->h < H_VAL_SECTION*3) {
        p_rgb->r = 0;
        p_rgb->g = G_VAL_LIMIT;
        p_rgb->b = map(p_hsv->h, H_VAL_SECTION*2, H_VAL_SECTION*3, 0, B_VAL_LIMIT);
    } else if (p_hsv->h < H_VAL_SECTION*4) {
        p_rgb->r = 0;
        p_rgb->g = map(p_hsv->h, H_VAL_SECTION*3, H_VAL_SECTION*4, G_VAL_LIMIT, 0);
        p_rgb->b = B_VAL_LIMIT;
    } else if (p_hsv->h < H_VAL_SECTION*5) {
        p_rgb->r = map(p_hsv->h, H_VAL_SECTION*4, H_VAL_SECTION*5, 0, R_VAL_LIMIT);
        p_rgb->g = 0;
        p_rgb->b = B_VAL_LIMIT;
    } else if (p_hsv->h < H_VAL_SECTION*6) {
        p_rgb->r = R_VAL_LIMIT;
        p_rgb->g = 0;
        p_rgb->b = map(p_hsv->h, H_VAL_SECTION*5, H_VAL_SECTION*6, B_VAL_LIMIT, 0);
    } else {
        p_rgb->r = 0;
        p_rgb->g = 0;
        p_rgb->b = 0;
    }
    
    // Modify for Saturation
    p_hsv->s = min(p_hsv->s, S_VAL_LIMIT);
    if(p_hsv->s < S_VAL_LIMIT) {
        p_rgb->r = map(p_hsv->s, 0, S_VAL_LIMIT, R_VAL_LIMIT, p_rgb->r); 
        p_rgb->g = map(p_hsv->s, 0, S_VAL_LIMIT, G_VAL_LIMIT, p_rgb->g); 
        p_rgb->b = map(p_hsv->s, 0, S_VAL_LIMIT, B_VAL_LIMIT, p_rgb->b); 
    }
    
    // Modify for Vibrancy
    p_hsv->v = min(p_hsv->v, V_VAL_LIMIT);
    if (p_hsv->v < V_VAL_LIMIT) {
        p_rgb->r = map(p_hsv->v, 0, V_VAL_LIMIT, 0, p_rgb->r); 
        p_rgb->g = map(p_hsv->v, 0, V_VAL_LIMIT, 0, p_rgb->g); 
        p_rgb->b = map(p_hsv->v, 0, V_VAL_LIMIT, 0, p_rgb->b); 
    }
    return p_rgb;
}

hsva_t* rgb_to_hsv(rgba_t* p_rgb, hsva_t* p_hsv)
{
    p_hsv->a = p_rgb->a;
    cval_t maxVal = max(p_rgb->r, max(p_rgb->g, p_rgb->b));
    cval_t minVal = min(p_rgb->r, min(p_rgb->g, p_rgb->b));
    
    // Vibrancy
    p_hsv->v = map(maxVal, 0, RGB_VAL_LIMIT, 0, V_VAL_LIMIT);
    p_hsv->v = constrain(p_hsv->v, 0, V_VAL_LIMIT);
    
    // Saturation
    p_hsv->s = map(minVal, 0, maxVal, S_VAL_LIMIT, 0);
    p_hsv->s = constrain(p_hsv->s, 0, S_VAL_LIMIT);
    
    // Hue
    if (p_rgb->b == minVal && p_rgb->r == maxVal)
    {
        p_hsv->h = map(p_rgb->g, minVal, maxVal, H_VAL_SECTION*0, H_VAL_SECTION*1);
    }
    else if (p_rgb->b == minVal && p_rgb->g == maxVal)
    {
        p_hsv->h = map(p_rgb->r, maxVal, minVal, H_VAL_SECTION*1, H_VAL_SECTION*2);
    }
    else if (p_rgb->r == minVal && p_rgb->g == maxVal)
    {
        p_hsv->h = map(p_rgb->b, minVal, maxVal, H_VAL_SECTION*2, H_VAL_SECTION*3);
    }
    else if (p_rgb->r == minVal && p_rgb->b == maxVal)
    {
        p_hsv->h = map(p_rgb->g, maxVal, minVal, H_VAL_SECTION*3, H_VAL_SECTION*4);
    }
    else if (p_rgb->g == minVal && p_rgb->b == maxVal)
    {
        p_hsv->h = map(p_rgb->r, minVal, maxVal, H_VAL_SECTION*4, H_VAL_SECTION*5);
    }
    else if (p_rgb->g == minVal && p_rgb->r == maxVal)
    {
        p_hsv->h = map(p_rgb->b, maxVal, minVal, H_VAL_SECTION*5, H_VAL_SECTION*6);
    }
    p_hsv->h = constrain(p_hsv->h, 0, H_VAL_LIMIT);
    
    return p_hsv;
}


