
#include <string.h>
#include "display_ripple.h"

namespace DispFunc_ripple {
    
Layer* layerPtr_ripple;

//rgba_t ripple_baseColor = {0, 0, B_VAL_LIMIT};
hsva_t ripple_baseColor = {0, S_VAL_LIMIT, V_VAL_LIMIT, A_VAL_LIMIT};
hsva_t ripple_waveColor = ripple_baseColor;
int ripple_waveDuration = 50;
int ripple_waveHold = 50;
int ripple_fadeDuration = 1000;
int ripple_startHold = 2000;

int ripple_waitForProp = 0;

static dir_t ripple_memo[PIXEL_COUNT];

void rippleWaveCallback(Pixel*);
void rippleDoneCallback(Pixel*);

void rippleWaveCallback(Pixel* p)
{
    dir_t dir = ripple_memo[p->getIndex()];
    if (dir == NULL) {
        return;
    }
    Pixel* next = layerPtr_ripple->getLinAdjacent(p, dir, 1);
    do {
        if (next) {
            next->
                    setCallback(rippleWaveCallback)->
                    setDuration(ripple_waveDuration)->
                    setHold(0)->
                    setTransition(&ripple_waveColor, millis());
            ripple_memo[next->getIndex()] = dir;
            ripple_waitForProp++;
        }
        do {
            dir = dir_CW(dir, 1);
            next = layerPtr_ripple->getLinAdjacent(p, dir, 1);
        } while (!next);
    } while (ripple_memo[next->getIndex()] == 0);
    
    p->
        setCallback(rippleDoneCallback)->
        setDuration(ripple_fadeDuration)->
        setHold(0)->
        setTransition(&ripple_baseColor, millis() + ripple_waveHold);
}
void rippleDoneCallback(Pixel* p)
{
    p->unsetCallback();
    ripple_waitForProp--;
}

void loop(funcState_t state)
{
    static Pixel* center;
    
    switch(state)   {
        case STATE_INIT: {
            layerPtr_ripple = Display::getAvailableLayer();
            ripple_baseColor.a = A_VAL_LIMIT;
            center = layerPtr_ripple->getCenterPixel();
            for (int i = 0; i < PIXEL_COUNT; i++) {
                layerPtr_ripple->getPixel(i)->
                    setDuration(100)->
                    setHold(0)->
                    setTransition(&ripple_baseColor, millis() + ripple_startHold);
            }
            ripple_waitForProp = 0;
            break;
        }
        case STATE_RUN: {
            if (ripple_waitForProp > 0)
            {   /* Wave is still propagating */
                return;
            }
            ripple_waveColor = ripple_baseColor;
            ripple_waveColor.s = S_VAL_LIMIT/4;
            memset(ripple_memo, 0, sizeof(ripple_memo));
            
            ripple_memo[center->getIndex()] = DIR_N;
            center->
                setCallback(rippleWaveCallback)->
                setDuration(ripple_waveDuration)->
                setHold(0)->
                setTransition(&ripple_waveColor, millis() + ripple_startHold);
            ripple_waitForProp++;
            
            center = layerPtr_ripple->getRandomPixel();
            ripple_baseColor.h += randomRange(H_VAL_SECTION/2, H_VAL_SECTION);
            if (ripple_baseColor.h >= H_VAL_LIMIT) {
                ripple_baseColor.h -= H_VAL_LIMIT;
            }
            break;
        }
        case STATE_DEINIT: {
            ripple_waitForProp = 0;
            ripple_waveColor = ripple_baseColor;
            ripple_waveColor.s = S_VAL_LIMIT/4;
            memset(ripple_memo, 0, sizeof(ripple_memo));
            ripple_memo[center->getIndex()] = DIR_N;
            
            ripple_baseColor.a = 0;
            center = layerPtr_ripple->getCenterPixel();
            center->
                setCallback(rippleWaveCallback)->
                setDuration(ripple_waveDuration)->
                setHold(0)->
                setTransition(&ripple_waveColor, millis());
            ripple_waitForProp++;
            break;
        }
    }
    
}
    
}
