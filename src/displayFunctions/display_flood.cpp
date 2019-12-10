#include <string.h>
#include "display_flood.h"

namespace DispFunc_flood {

Layer* layer;
bool flooded[PIXEL_COUNT];
    
rgba_t dropColor = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, A_VAL_LIMIT/4};
rgba_t dropColorFade = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, 0};

void newDrop(Pixel*); 
void dropFadeCallback(Pixel*);  

int dropIntervalMin = 250;
int dropIntervalMax = 500;
int dropWait = 0;
int dropDur = 50;
int dropHold = 0;
int dropFadeWait = 50;
int dropFadeDur = 50;

void newDrop(Pixel* p)
{
    int idx = p->getIndex();
    p->
        setDuration(dropDur)->
        setHold(dropHold)->
        setTransition(&dropColor, millis() + dropWait)->
        setCallback(dropFadeCallback);
}
void dropFadeCallback(Pixel* p)
{
    int idx = p->getIndex();
    Pixel* next;
    static dir_t dirs[] = {DIR_S, DIR_SW, DIR_SE};
    if (random(2)) {
        dir_t tmp = dirs[1];
        dirs[1] = dirs[2];
        dirs[2] = tmp;
    }
        
    for(int d = 0; d < sizeof(dirs)/sizeof(dir_t); d++) {
        next = layer->getAdjacent(p, dirs[d]);
        if (
            p->hasAdjacent(dirs[d]) && 
            next && 
            !flooded[next->getIndex()]
        )
        {
            // move dirs[d]
            p->
                setDuration(dropFadeDur)->
                setHold(0)->
                setTransition(&dropColorFade, millis() + dropFadeWait)->
                unsetCallback();
            
            newDrop(next);
            
            return;
        }
    }
    flooded[idx] = true;
}

void loop(funcState_t state)
{
    static uint32_t nextDropTime;
    switch(state) {
        case STATE_INIT: {
            layer = Display::getAvailableLayer();
            memset(&flooded, false, sizeof(flooded));
            nextDropTime = 0;
            break;
        }
        case STATE_RUN: {
            if (millis() > nextDropTime) {
                /* Generate a new drop */
                nextDropTime = randomRange(dropIntervalMin, dropIntervalMax) + millis();
                
                // /*
                int randStartPos = random(HEX_SIDE_LENGTH * 2);
                dir_t randStartDir = DIR_NE;
                Pixel* randStartPixel = layer->getCornerPixel(DIR_NW);
                while (randStartPos-- > 0) {
                    if (! randStartPixel->hasAdjacent(randStartDir)) {
                        randStartDir = dir_CW(randStartDir, 1);
                    }
                    randStartPixel = layer->getAdjacent(randStartPixel, randStartDir);
                }
                newDrop(randStartPixel);
                // */
                // newDrop(layer->getCornerPixel(DIR_N));
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}

} // namespace
