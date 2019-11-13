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
Layer layer_ripple;
Layer* layerPtr_ripple = &layer_ripple;

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

void ripple(funcState_t state)
{
    static Pixel* center;
    
    switch(state)   {
        case STATE_INIT: {
            layer_ripple.moveToTop();
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

rgba_t red = {R_VAL_LIMIT, 0, 0, A_VAL_LIMIT};
rgba_t green = {0, G_VAL_LIMIT, 0, A_VAL_LIMIT};
rgba_t blue = {0, 0, B_VAL_LIMIT, A_VAL_LIMIT};
rgba_t off = {0, 0, 0, 0};
    
Layer layer_rgb = Layer();
void rgbRepeat(funcState_t state)
{
    static uint32_t lastUpdate = 0;
    static Layer* layerPtr_rgb;
    switch(state) {
        case STATE_INIT: {
            layer_rgb.moveToTop();
            layerPtr_rgb = &layer_rgb;
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


Layer layer_snow = Layer();
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
        snow_newFlake(layer_snow.getAdjacent(p, DIR_S));
    }        
    p->
        setDuration(flakeFadeDur)->
        setHold(0)->
        setTransition(&flakeColorFade, millis() + flakeFadeWait)->
        unsetCallback();
}

void snow(funcState_t state)
{
    
    static const int MAX_FLAKES = 64;
    static int numFlakes = 0;
    static int lastFlake = 0;
    static uint32_t snowflakes[MAX_FLAKES];
    static uint32_t lastUpdate = 0;
    static int nextFlakeTime;
    switch(state) {
        case STATE_INIT: {
            layer_snow.moveToTop();
            numFlakes = 0;
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
                Pixel* randStartPixel = layer_snow.getCornerPixel(DIR_NW);
                while (randStartPos-- > 0) {
                    if (! randStartPixel->hasAdjacent(randStartDir)) {
                        randStartDir = dir_CW(randStartDir, 1);
                    }
                    randStartPixel = layer_snow.getAdjacent(randStartPixel, randStartDir);
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

#endif

#include "displayFunctions/display_clock.h"
#if 0
#include "displayFunctions/display_snake.h"
#include "displayFunctions/display_shimmer.h"
#endif

displayFunc_t displayFunctions[] = 
{
    //clockLoop,
    snow,
    ripple,
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
