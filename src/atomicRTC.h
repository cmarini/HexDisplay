#ifndef _ATOMICRTC_h
#define _ATOMICRTC_h

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile bool rtcSecTick;
extern void (*rtcSecTickCallback)(void);
    
extern void atomicRTC_init(void);
extern void startAtomicSync(void);
extern void stopAtomicSync(void);
extern bool isAtomicSyncing(void);
     
extern int getAtomicSSI(void);
extern int getFrameBitCount(void);
    
extern struct tm* RTCtoLocaltime(struct tm* time);
extern time_t RTCtoTime(void);
    
#ifdef __cplusplus
}
#endif

#endif /* _ATOMICRTC_h */

