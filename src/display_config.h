#ifndef _DISPLAY_CONFIG_h
#define _DISPLAY_CONFIG_h

#include "mymath.h"



#define GAMMA_LUT_SIZE RGB_VAL_LIMIT

#define HEX_SIDE_LENGTH 8
#define PIXEL_COUNT ((HEX_SIDE_LENGTH-1)*HEX_SIDE_LENGTH*3+1)
#define PIXEL_ROW_COUNT (HEX_SIDE_LENGTH*2-1)

#define USE_TLC5958 false   
#if USE_TLC5958
    #include "tlc5958.h"
    #include "display_tlc5958.h"
#else
    #include "display_sk6812.h"
#endif


#define A_VAL_LIMIT RGB_VAL_LIMIT

#define H_VAL_SECTION (RGB_VAL_LIMIT/6) 
#define H_VAL_LIMIT (H_VAL_SECTION*6) // Evaluates to largest integer value divisible by 6
    #define H_VAL_RED       (H_VAL_SECTION*0)
    #define H_VAL_YELLOW    (H_VAL_SECTION*1)
    #define H_VAL_GREEN     (H_VAL_SECTION*2)
    #define H_VAL_CYAN      (H_VAL_SECTION*3)
    #define H_VAL_BLUE      (H_VAL_SECTION*4)
    #define H_VAL_PURPLE    (H_VAL_SECTION*5)
#define S_VAL_LIMIT RGB_VAL_LIMIT
#define V_VAL_LIMIT RGB_VAL_LIMIT


#define _PIXEL_MAX_CVAL max(H_VAL_LIMIT, \
                        max(S_VAL_LIMIT, \
                        max(V_VAL_LIMIT, \
                        max(R_VAL_LIMIT, \
                        max(G_VAL_LIMIT, \
                            B_VAL_LIMIT) \
                        ))))

#if _PIXEL_MAX_CVAL > 0xFFFFFFFFu
    typedef uint64_t cval_t;
#elif _PIXEL_MAX_CVAL > 0xFFFFu 
    typedef uint32_t cval_t;
#elif _PIXEL_MAX_CVAL > 0xFFu 
    typedef uint16_t cval_t;
#else
    typedef uint8_t cval_t;
#endif

extern int pixelRowStart_lut[PIXEL_ROW_COUNT];
extern int pixelRowLength_lut[PIXEL_ROW_COUNT];
#define rc_to_i(r,c) (pixelRowStart_lut[(r)]+(c))

typedef enum {
    DIR_N  = 0x01,
    DIR_NE = 0x02,
    DIR_SE = 0x04,
    DIR_S  = 0x08,
    DIR_SW = 0x10,
    DIR_NW = 0x20,
    
    DIR_FIRST = DIR_N,
    DIR_LAST = DIR_NW,
} dir_t;

#define DIR_COUNT 6

typedef enum {
    EDGE_NE = 0x02,
    EDGE_E  = 0x03,
    EDGE_SE = 0x08,
    EDGE_SW = 0x20,
    EDGE_W  = 0x40,
    EDGE_NW = 0x80,
} edge_t;




typedef enum {
    DIFF_COLOR,
    DIFF_DURATION,
    DIFF_BOTH,
    DIFF_EITHER,
} PIXEL_SET_CONDITION_t;

typedef struct {
    cval_t r;
    cval_t g;
    cval_t b;
    cval_t a;
} rgba_t;
typedef struct {
    cval_t r;
    cval_t g;
    cval_t b;
} rgb_t;
typedef struct {
    cval_t h;
    cval_t s;
    cval_t v;
    cval_t a;
} hsva_t;   

typedef struct {
    uint8_t row;
    uint8_t col;
} coord_t;

#define USE_BRIGHTNESS_CORRECTION 1


#if USE_BRIGHTNESS_CORRECTION 
extern cval_t brightness_lut[];
extern const cval_t MIN_BRIGHTNESS_VAL;
#endif



#endif /* _DISPLAY_CONFIG_h */
