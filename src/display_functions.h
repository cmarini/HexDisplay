#ifndef _DISPLAY_FUNCTIONS_h
#define _DISPLAY_FUNCTIONS_h

#include "display.h"
#include "component/wiring.h" // For millis()
#include "atomicRTC.h"
#include "mymath.h"

typedef enum {
    STATE_INIT = 0,
    STATE_RUN,
    STATE_DEINIT,
} funcState_t;
typedef void (*displayFunc_t)(funcState_t);

typedef struct {
    displayFunc_t init;
    displayFunc_t func;
    displayFunc_t deinit;
} displayFuncSet_t;

extern const int NUM_DISPLAY_FUNCS;
extern displayFunc_t displayFunctions[];
extern bool activeDisplayFuncs[];

#endif /* _DISPLAY_FUNCTIONS_h */
