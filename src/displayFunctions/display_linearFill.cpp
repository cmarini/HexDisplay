
#include "display_linearFill.h"

namespace DispFunc_linearFill {

Layer* layerPtr;
rgba_t fillColor = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, A_VAL_LIMIT};
rgba_t fillColorFade = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, 0};

void fillCallback(Pixel*); 

int fillInterval = 100;
int fillTransitionIn = 200;
int fillHold = 2000;
int fillTransitionOut = 200;

void fillCallback(Pixel* p)
{   
    p->
        setDuration(fillTransitionOut)->
        setHold(0)->
        setTransition(&fillColorFade, millis())->
        unsetCallback();
}

void loop(funcState_t state)
{
    static uint32_t lastUpdate = 0;
    static uint32_t idx = 0;
    switch(state) {
        case STATE_INIT: {
            layerPtr = Display::getAvailableLayer();
            idx = 0;
            for (int i = 0; i < PIXEL_COUNT; i++) {
                layerPtr->getPixel(i)->
                    setDuration(0)->
                    setHold(0)->
                    setTransition(&fillColorFade, millis());
            }
            break;
        }
        case STATE_RUN: {
            if (millis() - lastUpdate > fillInterval) {
                lastUpdate = millis();
                layerPtr->getPixel(idx)->
                    setDuration(fillTransitionIn)->
                    setHold(fillHold)->
                    setTransition(&fillColor, millis()) ->
                    setCallback(fillCallback);
                idx++;
                idx = idx >= PIXEL_COUNT ? 0 : idx;
            }
            break;
        }
        case STATE_DEINIT: {
            break;
        }
    }
}

} // namespace
