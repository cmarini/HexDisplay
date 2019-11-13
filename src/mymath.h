#ifndef _MYMATH_h
#define _MYMATH_h

#include <math.h>
#include <stdlib.h>
#include <stdint.h>


#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif // min

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif // max

#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#ifndef round
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#endif

#ifndef sq
#define sq(x) ((x)*(x))
#endif

#define isOdd(x) ((x)%2)
#define isEven(x) (!(x)%2)


inline long map(long long x, long long in_min, long long in_max, long long out_min, long long out_max)
{
    /*
    if (in_max == in_min) // Make sure we don't divide by zero
    {
        return in_min;
    }
    */
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#ifdef __cplusplus
//extern "C" {
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void randomInit(void);
uint32_t getTRNG(void);
long random( long );
long randomRange( long, long );

#ifdef __cplusplus
}
#endif
    
#endif // _MYMATH_h
