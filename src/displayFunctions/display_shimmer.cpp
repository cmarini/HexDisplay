
#include "display_shimmer.h"

hsva_t shimmer_baseColor = {0, S_VAL_LIMIT, V_VAL_LIMIT};
int shimmerCount[PIXEL_COUNT];
const int shimmerCountMax = 20;
const int shimmerCountMin = 10;

void unshimmerCallback(Pixel*);
void shimmerCallback(Pixel* p)
{
    uint32_t idealNow = p->getIdealCompleteTime();
    int count = shimmerCount[p->getIndex()];
    count = max(1, count);
    
    hsva_t temp = shimmer_baseColor;
    temp.v = map(count, shimmerCountMax, 1, 0, V_VAL_LIMIT);
    
    p->
        setCallback(unshimmerCallback)->
        setHold(0)->
        setTransition(&temp, idealNow);
}
void unshimmerCallback(Pixel* p)
{
    uint32_t idealNow = p->getIdealCompleteTime();
    unsigned int idx = p->getIndex();
    shimmerCount[idx]--;
    
//    shimmer_baseColor.h += 2;
    while (shimmer_baseColor.h >= H_VAL_LIMIT) {
        shimmer_baseColor.h -= H_VAL_LIMIT;
    }
    
    p->
        setCallback(shimmerCallback)->
        setTransition(&shimmer_baseColor, idealNow);
    if (shimmerCount[idx] > 0) {
        p->
            setDuration((shimmerCount[idx]*shimmerCount[idx]/5)+0)->
            setHold(0);
    } else {
        p->
            setDuration(0)->
            setHold(randomRange(2000, 10000));
    }
        
    
    if (shimmerCount[idx] <= 0) {
        shimmerCount[idx] = randomRange(shimmerCountMin, shimmerCountMax);
    }
        
}

void shimmer(funcState_t state)
{
    static uint32_t prevUpdate = 0;
    switch(state)   {
        case STATE_INIT:
            rgba_t temp;
            for (int i = 0; i < PIXEL_COUNT; i++) {
                temp = pixels[i].getCurrent();
                pixels[i].
                    setCallback(shimmerCallback)->
                    setDuration(0)->
                    setHold(randomRange(0,5000))->
                    setTransition(&temp, millis());
                shimmerCount[i] = randomRange(shimmerCountMin, shimmerCountMax);
            }
        break;
        case STATE_RUN:
//            return;
            if (millis() - prevUpdate > 0) {
                prevUpdate = millis();
                shimmer_baseColor.h += 20;
                if (shimmer_baseColor.h >= H_VAL_LIMIT) {
                    shimmer_baseColor.h = 0;
                }
            }
        break;
        case STATE_DEINIT:
            for (int i = 0; i < PIXEL_COUNT; i++) {
                pixels[i].unsetCallback();
            }
        break;
    }
    
}

