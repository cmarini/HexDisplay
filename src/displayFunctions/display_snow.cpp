
#include "display_snow.h"

namespace DispFunc_snow {

Layer* layer_snow;
rgba_t flakeColor = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, A_VAL_LIMIT};
rgba_t flakeColorFade = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, 0};

void snow_newFlake(Pixel*); 
void snow_flakeFadeCallback(Pixel*);  

int flakeIntervalMin = 200;
int flakeIntervalMax = 1000;
int flakeWait = 0;
int flakeDur = 100;
int flakeHold = 0;
int flakeFadeWait = 150;
int flakeFadeDur = 600;

void snow_newFlake(Pixel* p)
{
    p->
        setDuration(flakeDur)->
        setHold(flakeHold)->
        setTransition(&flakeColor, millis() + flakeWait)->
        setCallback(snow_flakeFadeCallback);
}
void snow_flakeFadeCallback(Pixel* p)
{
    if (p->hasAdjacent(DIR_S)) {
        snow_newFlake(layer_snow->getAdjacent(p, DIR_S));
    }        
    p->
        setDuration(flakeFadeDur)->
        setHold(0)->
        setTransition(&flakeColorFade, millis() + flakeFadeWait)->
        unsetCallback();
}

void loop(funcState_t state)
{
    
    static int lastFlake = 0;
    static int nextFlakeTime;
    switch(state) {
        case STATE_INIT: {
            layer_snow = Display::getAvailableLayer();
            lastFlake = -1;
            nextFlakeTime = 0;
            break;
        }
        case STATE_RUN: {
            if (millis() > nextFlakeTime) {
                /* Generate a new flake */
                nextFlakeTime = randomRange(flakeIntervalMin, flakeIntervalMax) + millis();
                lastFlake++;
                
                int randStartPos = random(HEX_SIDE_LENGTH * 2);
                dir_t randStartDir = DIR_NE;
                Pixel* randStartPixel = layer_snow->getCornerPixel(DIR_NW);
                while (randStartPos-- > 0) {
                    if (! randStartPixel->hasAdjacent(randStartDir)) {
                        randStartDir = dir_CW(randStartDir, 1);
                    }
                    randStartPixel = layer_snow->getAdjacent(randStartPixel, randStartDir);
                }
                snow_newFlake(randStartPixel);
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}

} // namespace
