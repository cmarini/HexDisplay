#include "display_functions.h"
#include "TimerOne.h"
#include <stdio.h>
#include <string.h>

/*
    switch(state)   {
        case STATE_INIT: {
            break;
        }
        case STATE_RUN: {
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
*/


#if 0


void brightnessRotate(funcState_t state)
{
    static hsva_t hsvA = {0,S_VAL_LIMIT,0};
    switch(state)   {
        case STATE_INIT: {
            getPixel(0)->stop();
            getPixel(0)->off();
            break;
        }
        case STATE_RUN: {     
            if (getPixel(0)->isTransitionComplete()) {
                if (getPixel(0)->getCurrentR() == 0 && getPixel(0)->getCurrentG() == 0 && getPixel(0)->getCurrentB() == 0) {
                    hsvA.v = V_VAL_LIMIT;
                    hsvA.h += H_VAL_SECTION*2;
                    if (hsvA.h >= H_VAL_LIMIT) {
                        hsvA.h = 0;
                    }
                } else {
                    hsvA.v = 0;
                }
                for (int i = 0; i < PIXEL_COUNT; i++) {
                    getPixel(i)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                }
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}
void hueRotate(funcState_t state)
{
    static hsva_t hsvA = {0,S_VAL_LIMIT,V_VAL_LIMIT};
    rgba_t white = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT};
    switch(state)   {
        case STATE_INIT: {
            getPixel(0)->stop();
            getPixel(0)->
                setDuration(100)->
                setHold(0)->
                setColor(&white);
            break;
        }
        case STATE_RUN: {     
            if (getPixel(0)->isTransitionComplete()) {
                hsvA.h += H_VAL_SECTION;
                if (hsvA.h >= H_VAL_LIMIT) {
                    hsvA.h = 0;
                }
                for (int i = 0; i < PIXEL_COUNT; i++) {
                    getPixel(i)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                }
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}


void randomHue_callback(Pixel* pixel)
{
    static hsva_t hsvA = {0,S_VAL_LIMIT,V_VAL_LIMIT};
    hsvA.h = random(H_VAL_LIMIT);
    pixel->
        setDuration(randomRange(250, 1000))->
        setHold(randomRange(1000, 2000))->
        setTransition(&hsvA, millis());
}
void randomHue(funcState_t state)
{
    switch(state)   {
        case STATE_INIT: {
            for (int i = 0; i < PIXEL_COUNT; i++) {
                getPixel(i)->setCallback(randomHue_callback)->stop()->uncomplete();
            }
            break;
        }
        case STATE_RUN: {
            break;
        }
        case STATE_DEINIT: {
            for (int i = 0; i < PIXEL_COUNT; i++) {
                getPixel(i)->unsetCallback();
            }
            break;
        }
    }
}

void white(funcState_t state)
{
    static uint32_t lastUpdate = 0;
    rgba_t white = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT};
    switch(state)   {
        case STATE_INIT: {
            for (int i = 0; i < PIXEL_COUNT; i++) {
                getPixel(i)->
                    setDuration(250)->
                    setHold(0)->
                    setTransition(&white, millis());
            }
            break;
        }
        case STATE_RUN: {
            if (millis() - lastUpdate > 1000) {
                lastUpdate = millis();
                //TLC.WriteFC1();
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}

volatile bool do_oneSecRot = true;
void oneSecRot_callback(uint32_t status)
{
    do_oneSecRot = true;
}
void oneSecRot(funcState_t state)
{
    switch(state)   {
        case STATE_INIT: {
            for (int i = 0; i < PIXEL_COUNT; i++) {
                getPixel(i)->
                    unsetCallback()->
                    uncomplete();
            }
            Timer1.initialize(CHANNEL0);
            Timer1.setPeriod(CHANNEL0, 1000000L);
            Timer1.attachInterrupt(CHANNEL0, oneSecRot_callback);
            Timer1.start(CHANNEL0);
            do_oneSecRot = true;
            break;
        }
        case STATE_RUN: {
            if (do_oneSecRot) {
                do_oneSecRot = false;
                static hsva_t hsvA = {0,S_VAL_LIMIT,V_VAL_LIMIT};
                hsvA.h += H_VAL_SECTION;
                if (hsvA.h >= H_VAL_LIMIT) {
                    hsvA.h = 0;
                }
                for (int i = 0; i < PIXEL_COUNT; i++) {
                    getPixel(i)->
                        setDuration(100)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                }
            }
            break;
        }
        case STATE_DEINIT: {
            Timer1.stop(CHANNEL0);
            Timer1.detachInterrupt(CHANNEL0);
            break;
        }
    }
}
    
void ssi(funcState_t state)
{
    static rgba_t ssi_color = {0, 0, 0};
    switch(state)   {
        case STATE_INIT: {
            for (int i = 0; i < PIXEL_COUNT; i++) {
                    getPixel(i)->stop();
            }
            break;
        }
        case STATE_RUN: {
            if (getPixel(0)->isTransitionComplete())
            {
                int ssi = getAtomicSSI();
                ssi_color.r = map(constrain(ssi,  80, 100),  80, 100, R_VAL_LIMIT, 0);
                ssi_color.g = map(constrain(ssi,  20,  80),  20,  80, 0, G_VAL_LIMIT);
                rgba_t target = getPixel(0)->getTarget();
                if (!Pixel::areEqualRGB(&target, &ssi_color))
                {
                    for (int i = 0; i < PIXEL_COUNT; i++) {
                        getPixel(i)->
                            setDuration(900)->
                            setHold(0)->
                            setTransition(&ssi_color, millis());
                    }   
                }
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}


#endif

#if 0
void brightnessRotateRGB(funcState_t state)
{
    static Layer layerPtr_rgb;
    static Layer fg;
    static hsva_t hsvA = {0, S_VAL_LIMIT, V_VAL_LIMIT, A_VAL_LIMIT};
    static rgba_t black = {0, 0, 0, A_VAL_LIMIT};
    static rgba_t bgColor = {0, 0, MIN_BRIGHTNESS_VAL, A_VAL_LIMIT};
    static rgba_t layerPtr_ripple = {0, 0, B_VAL_LIMIT, A_VAL_LIMIT};
    static Pixel* center = layerPtr_rgb.getCenterPixel();
    switch(state)   {
        case STATE_INIT: {
            for (int i = 0; i < PIXEL_COUNT; i++) {
                layerPtr_rgb.getPixel(i)->
                    setDuration(1000)->
                    setHold(0)->
                    setTransition(&bgColor, millis() + 10 * i);
            }
            break;
        }
        case STATE_RUN: {     
            if (center->isTransitionComplete()) {
                if (center->getCurrentA() == 0) {
                    hsvA.a = A_VAL_LIMIT;
                } else {
                    hsvA.a = 0;
                }
                hsvA.h = 0;
                hsvA.s = 0;
                center->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                hsvA.h = H_VAL_RED;
                hsvA.s = S_VAL_LIMIT;
                // fg.getLinAdjacent(center, DIR_N, 4)->
                layerPtr_rgb.getPixel(0)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&layerPtr_ripple, millis());
                layerPtr_rgb.getPixel(1)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&layerPtr_ripple, millis());
                fg.getPixel(0)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                fg.getLinAdjacent(fg.getPixel(0), DIR_S, 1)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                hsvA.h = H_VAL_GREEN;
                hsvA.s = S_VAL_LIMIT;
                fg.getLinAdjacent(center, DIR_SE, 4)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
                hsvA.h = H_VAL_BLUE;
                hsvA.s = S_VAL_LIMIT;
                fg.getLinAdjacent(center, DIR_SW, 4)->
                        setDuration(5000)->
                        setHold(0)->
                        setTransition(&hsvA, millis());
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}
#endif

#if 1

rgba_t red = {R_VAL_LIMIT, 0, 0, A_VAL_LIMIT};
rgba_t green = {0, G_VAL_LIMIT, 0, A_VAL_LIMIT};
rgba_t blue = {0, 0, B_VAL_LIMIT, A_VAL_LIMIT};
rgba_t off = {0, 0, 0, 0};
    
//Layer layer_rgb = Layer();
void rgbRepeat(funcState_t state)
{
    static uint32_t lastUpdate = 0;
    static Layer* layerPtr_rgb;
    switch(state) {
        case STATE_INIT: {
            layerPtr_rgb = Display::getAvailableLayer();
            for (int i = 0; i < PIXEL_COUNT-4; i+=4) {
                layerPtr_rgb->getPixel(i)->
                    setDuration(0)->
                    setHold(0)->
                    setTransition(&red, millis());
                layerPtr_rgb->getPixel(i+1)->
                    setDuration(0)->
                    setHold(0)->
                    setTransition(&green, millis());
                layerPtr_rgb->getPixel(i+2)->
                    setDuration(0)->
                    setHold(0)->
                    setTransition(&blue, millis());
                layerPtr_rgb->getPixel(i+3)->
                    setDuration(0)->
                    setHold(0)->
                    setTransition(&off, millis());
            }
            break;
        }
        case STATE_RUN: {
            if (millis() - lastUpdate > 1000) {
                lastUpdate = millis();
                //TLC.WriteFC1();
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}


#endif

#include "displayFunctions/display_snow.h"
#include "displayFunctions/display_clock.h"
#include "displayFunctions/display_ripple.h"
#include "displayFunctions/display_flood.h"
#if 0
#include "displayFunctions/display_snake.h"
#include "displayFunctions/display_shimmer.h"
#endif

displayFunc_t displayFunctions[] = 
{
    //clockLoop,
    DispFunc_flood::loop,
    DispFunc_ripple::loop,
    DispFunc_clock::loop,
    DispFunc_snow::loop,
	rgbRepeat,
//    brightnessRotateRGB,
#if 0
    snakeLoop,
    brightnessRotate,
    hueRotate,
	rgbRepeat,
    shimmer,
//    oneSecRot,
//    ssi,
//    white,
//    randomHue,
#endif
};

const int NUM_DISPLAY_FUNCS = sizeof(displayFunctions)/sizeof(displayFunc_t);

bool activeDisplayFuncs[NUM_DISPLAY_FUNCS];
