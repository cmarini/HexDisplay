#ifndef _DISPLAY_TLC5958_h
#define _DISPLAY_TLC5958_h

#include <stdint.h>
#include <stddef.h>

#define R_VAL_LIMIT GS_OUTR_Max
#define G_VAL_LIMIT GS_OUTG_Max
#define B_VAL_LIMIT GS_OUTB_Max
#define RGB_VAL_LIMIT GS_OUT_Max


#define NUM_CHAINED_TLC5958 2
#define COLS_PER_TLC5958 16

#define NUM_TLC_COLS        (COLS_PER_TLC5958*NUM_CHAINED_TLC5958)
#if (PIXEL_COUNT%NUM_TLC_COLS)>0
    #define NUM_TLC_ROWS (PIXEL_COUNT/NUM_TLC_COLS + 1)
#else
    #define NUM_TLC_ROWS (PIXEL_COUNT/NUM_TLC_COLS)
#endif
#define NUM_SCAN_LINES  NUM_TLC_ROWS


    #if NUM_TLC_COLS > (NUM_CHAINED_TLC5958 * 16)
        #error Too many columns.
    #endif
    #if NUM_TLC_ROWS > 32
        #error Too many rows.
    #endif


#define ES_PWM_DISPLAY_MODE     FC2_SEL_PWM_9P7
    #if (ES_PWM_DISPLAY_MODE == FC2_SEL_PWM_9P7)
        #define SEGMENT_TC_COUNT (513 * 2 + 1)
    #elif (ES_PWM_DISPLAY_MODE == FC2_SEL_PWM_8P8)
        #define SEGMENT_TC_COUNT (257 * 2 + 1)
    #else
        #error Invalid ES-PWM mode
    #endif


#define SCAN_LINE_PINS_PORT     PIOC
#define SCAN_LINE_PINS_OFFSET   1       // Pins PC1 - PC9 (Arduino 33-41)

#include "tlc5958.h"

extern TLC5958 TLC;

extern volatile bool doVsync;
void displayInterface_init();
void updateDisplay();

#endif /* _DISPLAY_TLC5958_h */
