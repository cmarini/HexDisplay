#ifndef _PIXEL_h
#define _PIXEL_h

#include "mymath.h"
#include "display_config.h"

typedef enum {
    DIR_N  = 0x01,
    DIR_NE = 0x02,
    DIR_SE = 0x04,
    DIR_S  = 0x08,
    DIR_SW = 0x10,
    DIR_NW = 0x20,
    
    DIR_FIRST = DIR_N,
    DIR_LAST = DIR_NW,
} dir_t;

#define DIR_COUNT 6

typedef enum {
    EDGE_NE = 0x02,
    EDGE_E  = 0x03,
    EDGE_SE = 0x08,
    EDGE_SW = 0x20,
    EDGE_W  = 0x40,
    EDGE_NW = 0x80,
} edge_t;




typedef enum {
    DIFF_COLOR,
    DIFF_DURATION,
    DIFF_BOTH,
    DIFF_EITHER,
} PIXEL_SET_CONDITION_t;

typedef struct {
    cval_t r;
    cval_t g;
    cval_t b;
    cval_t a;
} rgba_t;
typedef struct {
    cval_t r;
    cval_t g;
    cval_t b;
} rgb_t;
typedef struct {
    cval_t h;
    cval_t s;
    cval_t v;
    cval_t a;
} hsva_t;   

typedef struct {
    uint8_t row;
    uint8_t col;
} coord_t;
    

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

#endif /* _PIXEL_h */

