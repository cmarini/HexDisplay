#ifndef _LAYER_h
#define _LAYER_h

#include <stdint.h>
#include <stddef.h>
#include "display_config.h"
#include "pixel.h"

class Layer
{    
    public:
        // PROPERTIES
    
        // METHODS
        Layer();
        ~Layer();
        void init();
    
        static Layer* getBase();
        static Layer* getTop();
        static uint8_t getLayerCount();
        Layer* getBelow();
        Layer* getAbove();
        Layer* moveDown();
        Layer* moveUp();
        Layer* moveToTop();
        Layer* moveToBottom();
   
        Pixel* getAdjacent(Pixel*, dir_t);
        Pixel* getLinAdjacent(Pixel*, dir_t, unsigned int);
        Pixel* getPixel(uint8_t);
        Pixel* getPixel(uint8_t, uint8_t);
        Pixel* getRandomPixel();
        Pixel* getCenterPixel();
        Pixel* getCornerPixel(dir_t);
   
    private:
        // PROPERTIES
        Pixel pixels[PIXEL_COUNT];
        static Layer* base;
        static Layer* top;
        static uint8_t layerCount;
        Layer* below;
        Layer* above;
};

#endif /* _LAYER_h */
