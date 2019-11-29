#ifndef _PIXEL_h
#define _PIXEL_h

#include "mymath.h"
#include "display_config.h"
//#include "display.h"
    

class Pixel
{
    typedef void (*PixelCallback_t)(Pixel*);
    
    public:
        // PROPERTIES
        coord_t loc;
        uint8_t adjacency;
    
        uint32_t duration;
        uint32_t hold;
    
        // METHODS
        Pixel();
    
        static bool areEqualRGB(rgba_t*, rgba_t*);
    
        void update(uint32_t timeNow);
        bool hasAdjacent(dir_t);
        bool isEdge();
        int getIndex();
    
        Pixel* off();
        Pixel* setColor(rgba_t*);
        Pixel* setTransition(rgba_t*, uint32_t start);
        bool setTransitionIf(rgba_t*, uint32_t start, uint32_t duration, PIXEL_SET_CONDITION_t condition);
    
        Pixel* setColor(hsva_t*);
        Pixel* setTransition(hsva_t*, uint32_t start);
        bool setTransitionIf(hsva_t*, uint32_t start, uint32_t duration, PIXEL_SET_CONDITION_t condition);
    
        Pixel* setCallback(PixelCallback_t);
        Pixel* unsetCallback();
        Pixel* doCallback();
    
        bool isTransitionComplete();
        Pixel* uncomplete();
        Pixel* stop();
        Pixel* setDuration(uint32_t);
        Pixel* setHold(uint32_t);
        uint32_t getIdealCompleteTime();
        
        rgba_t getTarget();
        cval_t getTargetR();
        cval_t getTargetG();
        cval_t getTargetB();
        cval_t getTargetA();
        
        rgba_t getCurrent();
        cval_t getCurrentR();
        cval_t getCurrentG();
        cval_t getCurrentB();
        cval_t getCurrentA();
    
    protected:
        // PROPERTIES
        PixelCallback_t callback;
        bool complete;
        
        rgba_t current;
        rgba_t start;
        rgba_t target;
        
        uint32_t startTime;
};


rgba_t* hsv_to_rgb(hsva_t*, rgba_t*);
hsva_t* rgb_to_hsv(rgba_t*, hsva_t*);

#endif /* _PIXEL_h */

