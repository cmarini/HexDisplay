
#include "display_clock.h"

const int sc_segLen = 7;
const int mn_segLen = 5;
const int sync_segLen = 3;
const int hr_segLen = 1;

const int mn_segMax = 60/6;
const int sc_segMax = 60/6;
const int hr_segMax = 12/6;

void clockSecCallback(Pixel* p)
{
    rgba_t black = {0, 0, 0};
    p->
        unsetCallback()->
        setDuration(sc_segMax*1000/sc_segLen)->
        setHold(0)->
        setTransition(&black, millis());
}
void clockMinCallback(Pixel* p)
{
    rgba_t black = {0, 0, 0};
    p->
        unsetCallback()->
        setDuration(1000)->
        setHold(0)->
        setTransition(&black, millis());
}

Layer* face;
void clockLoop(funcState_t state)
{
    //static Layer faceLayer;
    //static Layer* face = &faceLayer;
    static bool firstRun;
    uint32_t now;
    rgba_t black = {0, 0, 0, 0};
    rgba_t white = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT, A_VAL_LIMIT};
    hsva_t hsv_offColor = {0, 0, 0, 0};
    hsva_t hsv_hrColor = {H_VAL_RED, S_VAL_LIMIT, V_VAL_LIMIT, A_VAL_LIMIT};
    hsva_t hsv_mnColor = {H_VAL_GREEN, S_VAL_LIMIT, V_VAL_LIMIT, A_VAL_LIMIT};
    hsva_t hsv_scColor = {H_VAL_BLUE, S_VAL_LIMIT, V_VAL_LIMIT, A_VAL_LIMIT};
    hsva_t hsv_syncProgColor = {H_VAL_CYAN, S_VAL_LIMIT, V_VAL_LIMIT/4, A_VAL_LIMIT};
    hsva_t hsv_syncCompColor = {H_VAL_BLUE, S_VAL_LIMIT, V_VAL_LIMIT/4, A_VAL_LIMIT};
    const dir_t segDirs[] = {
        DIR_SE, DIR_S, DIR_SW, DIR_NW, DIR_N, DIR_NE,
    };
    
    static Pixel* hr_segStarts[6] = {};
        
    static Pixel* mn_segStarts[6] = {};
        
    static Pixel* sc_segStarts[6] = {};
        
    static Pixel* sync_segStarts[6] = {};
        
    static Pixel* ssi_pixel;
        
    static Pixel* transitionSentinel;
        
    switch(state)   {
        case STATE_INIT: {
            face = Layer::getTop();
            firstRun = true;
            for (int i = 0; i < PIXEL_COUNT; i++) {
                face->getPixel(i)->
                    setDuration(1000)->
                    setHold(0)->
                    setTransition(&black, millis() + 10 * i);
                transitionSentinel = face->getPixel(i);
            }
            /* Initialize segment lookups */
            ssi_pixel = face->getCenterPixel();
            hr_segStarts[0] = face->getLinAdjacent(ssi_pixel, DIR_N, hr_segLen);
            mn_segStarts[0] = face->getLinAdjacent(ssi_pixel, DIR_N, mn_segLen);
            sc_segStarts[0] = face->getLinAdjacent(ssi_pixel, DIR_N, sc_segLen);
            sync_segStarts[0] = face->getLinAdjacent(ssi_pixel, DIR_N, sync_segLen);
                
            for (int i = 1; i < 6; i++) {
                hr_segStarts[i] = face->getLinAdjacent(hr_segStarts[i-1], segDirs[i-1], hr_segLen);
                mn_segStarts[i] = face->getLinAdjacent(mn_segStarts[i-1], segDirs[i-1], mn_segLen);
                sc_segStarts[i] = face->getLinAdjacent(sc_segStarts[i-1], segDirs[i-1], sc_segLen);
                sync_segStarts[i] = face->getLinAdjacent(sync_segStarts[i-1], segDirs[i-1], sync_segLen);
            }
            break;
        }
        case STATE_RUN: {
            if (!rtcSecTick) {
                return;
            }
            if (firstRun && !transitionSentinel->isTransitionComplete()) {
                return;
            }
            now = millis();
            rtcSecTick = false;
            struct tm time;
            RTCtoLocaltime(&time);
            time.tm_sec++;
            mktime(&time);
            
            /* Clear all clock pixels */
            int h_i, m_i, s_i, sy_i;
            for (int i = 0; i < 0; i++) 
            {   /* Iterate segments */
                for (h_i = 0; h_i <= hr_segLen; h_i++) {
                    face->
                        getLinAdjacent(hr_segStarts[i], segDirs[i], h_i)->
                        setTransition(&hsv_offColor, millis());
                }
                for (m_i = 0; m_i <= mn_segLen; m_i++) {
                    face->
                        getLinAdjacent(mn_segStarts[i], segDirs[i], m_i)->
                        setTransition(&hsv_offColor, millis());
                }
                for (s_i = 0; s_i <= sc_segLen; s_i++) {
                    face->
                        getLinAdjacent(sc_segStarts[i], segDirs[i], s_i)->
                        setTransition(&hsv_offColor, millis());
                }
                for (sy_i = 0; sy_i <= sync_segLen; sy_i++) {
                    face->
                        getLinAdjacent(sync_segStarts[i], segDirs[i], sy_i)->
                        setTransition(&hsv_offColor, millis());
                }
            }
            ssi_pixel->setTransition(&hsv_offColor, millis());
            
            int segment;
            int segPerc;
            int pixelCount;
            int pixelPerc;
            #if 1
            /* Set hour pixels */
            static const int hr_segMax = 60*2;
            segment = (time.tm_hour % 12) / 2;
            segPerc = (time.tm_min + (60 * (time.tm_hour%12 - (segment*2)))) * 100 / hr_segMax;
            pixelCount = segPerc * hr_segLen / 100;
            pixelPerc = 100 - ((segPerc * hr_segLen) % 100);
            
            hsv_hrColor.v = map(pixelPerc, 0, 100, 0, S_VAL_LIMIT);
            face->getLinAdjacent(hr_segStarts[segment], segDirs[segment], pixelCount)->
                setTransition(&hsv_hrColor, millis());
            hsv_hrColor.v = map(pixelPerc, 100, 0, 0, S_VAL_LIMIT);
            face->getLinAdjacent(hr_segStarts[segment], segDirs[segment], pixelCount+1)->
                setTransition(&hsv_hrColor, millis());
            
            #endif
            /* Set minute pixels */
            segment = (time.tm_min / mn_segMax) % 6;
            segPerc = (((time.tm_min - (segment*mn_segMax)))) * 100 / mn_segMax;
            pixelCount = (segPerc * (mn_segLen)) / 100;
            pixelPerc = 100 - ((segPerc * 100 / (mn_segLen-1)) % 100);
            
            hsv_mnColor.v = map(pixelPerc, 0, 100, 0, S_VAL_LIMIT);
            face->getLinAdjacent(mn_segStarts[segment], segDirs[segment], pixelCount)->
                setTransition(&hsv_mnColor, millis())->
                setDuration(1000)->
                setHold(0)->
                setCallback(clockMinCallback);
            hsv_mnColor.v = map(100-pixelPerc, 0, 100, 0, S_VAL_LIMIT);
            face->getLinAdjacent(mn_segStarts[segment], segDirs[segment], pixelCount+1)->
                setTransition(&hsv_mnColor, millis())->
                setDuration(1000)->
                setHold(0)->
                setCallback(clockMinCallback);
            
/*
            segment = (time.tm_min / mn_segMax) % 6; // Which straight segment of the hex
            int seg_mn = time.tm_min - (segment*mn_segMax); // Number of minutes into the current segment
                       
            hsv_mnColor.v = V_VAL_LIMIT;
            if (seg_mn == 0 || firstRun) {
                for (int p = 0; p <= mn_segLen; p++) {
                    uint32_t pixelMaxTime = mn_segMax*60*1000*p/mn_segLen;
                    if (seg_mn*60*1000 < pixelMaxTime) {
                        mn_segStarts[segment]->getLinAdjacent(segDirs[segment], p)->
                            setTransition(&hsv_mnColor, now - (seg_mn*60*1000) + pixelMaxTime)->
                            setDuration(min(pixelMaxTime-(seg_mn*60*1000), mn_segMax*60*1000/mn_segLen))->
                            setHold(0)->
                            setCallback(clockMinCallback);
                    }
                }
            }
            */
            
            /* Set second pixels */
            segment = (time.tm_sec / sc_segMax) % 6; // Which straight segment of the hex
            int segSec = time.tm_sec - (segment*sc_segMax); // Number of seconds into the current segment
                        
            hsv_scColor.v = V_VAL_LIMIT;
            if (segSec == 0 || firstRun) {
                for (int p = 0; p <= sc_segLen; p++) {
                    uint32_t pixelMaxTime = sc_segMax*1000*p/sc_segLen;
                    if (segSec*1000 < pixelMaxTime) {
                        face->getLinAdjacent(sc_segStarts[segment], segDirs[segment], p)->
                            setTransition(&hsv_scColor, now - (segSec*1000) + pixelMaxTime)->
                            setDuration(min(pixelMaxTime-(segSec*1000), sc_segMax*1000/sc_segLen) - 00)->
                            setHold(250)->
                            setCallback(clockSecCallback);
                    }
                }
            }
            //int val = map(pixelPerc, 0, 100, 0, S_VAL_LIMIT);
            
            //hsv_scColor.v = map(pixelPerc, 0, 100, 0, S_VAL_LIMIT);
            //hsv_scColor.v = val;
            //sc_segStarts[segment]->getLinAdjacent(segDirs[segment], pixelCount)->
                //setTransition(&hsv_scColor, millis())->setCallback(clockSecCallback);
            //hsv_scColor.v = map(pixelPerc, 100, 0, 0, S_VAL_LIMIT);
            
            /* Set sync progress pixels */
            if (isAtomicSyncing())
            {
                int syncProg = getFrameBitCount();
                static const int sync_segMax = 10;
                segment = (syncProg / 10);
                segPerc = (syncProg - (segment*10)) * 100 / sync_segMax;
                pixelCount = segPerc * sync_segLen / 100;
                pixelPerc = 100 - ((segPerc * sync_segLen) % 100);
                
                if (segment < 6) {
                    hsv_syncProgColor.v = map(pixelPerc, 0, 100, 0, S_VAL_LIMIT/4);
                    sync_segStarts[segment]->
                        setTransition(&hsv_syncProgColor, now);
                } else {
                    segment = 6;
                }
                    
                for (int s = 0; s < segment; s++)
                {   /* Set completed pixels */
                    sync_segStarts[s]->
                        setTransition(&hsv_syncCompColor, now);
                }
                
                rgba_t ssi_color = {0, 0, 0};
                int ssi = getAtomicSSI();
                ssi_color.r = map(constrain(ssi,  80, 100),  80, 100, R_VAL_LIMIT/2, 0);
                ssi_color.g = map(constrain(ssi,  20,  80),  20,  80, 0, G_VAL_LIMIT/4);
                ssi_pixel->setTransition(&ssi_color, now);
            }
            else 
            {   /* Not syncing */
                hsva_t ssi_color = {0, 0, V_VAL_LIMIT/8};
                ssi_pixel->setTransition(&ssi_color, now)->
                setDuration(1000)->
                setHold(0);
                
                
                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < sync_segLen; j++) {
                        face->getLinAdjacent(sync_segStarts[i], segDirs[i], sync_segLen);
                    }
                }
            }
            firstRun = false;
            break;
        }
        case STATE_DEINIT: {
            rtcSecTickCallback = NULL;
            break;
        }
    }
}
