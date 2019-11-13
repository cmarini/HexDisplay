#ifndef TIMERONE_h
#define TIMERONE_h

#include <sam3xa.h>
#include <stdint.h>
#include "component/pmc.h"

// #define RESOLUTION 65536    // Timer1 is 16 bit

typedef enum
{
    CHANNEL0 = 0,
    CHANNEL1,
    CHANNEL2,
    
    CHANNEL_LAST = CHANNEL2 - 1,
} timerChannel_t;

typedef void (*TimerOneCallback_t)(uint32_t status);

class TimerOne
{
    public:

        // properties
        TimerOneCallback_t isrCallbacks[3];
        unsigned int pwmPeriod;
        unsigned char clockSelectBits;
        char oldSREG;					// To hold Status Register while ints disabled

        // methods
        void initialize(timerChannel_t channel);
        void configure(timerChannel_t channel, uint32_t mode);
        void start(timerChannel_t channel);
        void stop(timerChannel_t channel);
        void resume(timerChannel_t channel);
        unsigned long read(timerChannel_t channel);
        void attachInterrupt(timerChannel_t channel, TimerOneCallback_t isr);
        void detachInterrupt(timerChannel_t channel);
        void setPeriod(timerChannel_t channel, unsigned long microseconds);
        void setRA(timerChannel_t channel, uint32_t val);
        void setRB(timerChannel_t channel, uint32_t val);
        void setRC(timerChannel_t channel, uint32_t val);
        void setPwmDuty(char pin, int duty);
        void sync();
    
    private:
        
};

extern TimerOne Timer1;
#endif
