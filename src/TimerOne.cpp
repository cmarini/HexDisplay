#ifndef TIMERONE_cpp
#define TIMERONE_cpp

#include "TimerOne.h"
#include <stddef.h>
#include "wiring_digital.h"



TimerOne Timer1;              // preinstatiate


void TimerOne::initialize(timerChannel_t channel)
{
    if (channel > CHANNEL_LAST) {
        return;
    }
    
    isrCallbacks[channel] = NULL;
    
    configure(channel,
        TC_CMR_TCCLKS_TIMER_CLOCK1 | // CLOCK1 = MCK/2
        TC_CMR_WAVE |         // Waveform mode
        TC_CMR_WAVSEL_UP_RC | // Counter running up and reset when equals to RC
        TC_CMR_EEVT_XC0 |     // Set external events from XC0 (this setup TIOB as output)
        TC_CMR_ACPA_NONE | TC_CMR_ACPC_NONE |
        TC_CMR_BCPB_NONE | TC_CMR_BCPC_NONE
    );
    
    pmc_set_writeprotect(0); // disable PMC write protection
    pmc_enable_periph_clk(
        ID_TC0 + // ID of first TC module (TC0, channel 0)
        3 + // Gets us to ID of TC1 module (Skip TC0 channels 0-2)
        channel // Finally gets us to the ID of the this specific channel
    );
}

void TimerOne::configure(timerChannel_t channel, uint32_t mode)
{

    /*  Disable TC clock */
    TC1->TC_CHANNEL[channel].TC_CCR = TC_CCR_CLKDIS ;

    /*  Disable interrupts */
    TC1->TC_CHANNEL[channel].TC_IDR = 0xFFFFFFFF ;

    /*  Clear status register */
    uint32_t tmp = TC1->TC_CHANNEL[channel].TC_SR ;

    /*  Set mode */
    TC1->TC_CHANNEL[channel].TC_CMR = mode ;
}

void TimerOne::setRC(timerChannel_t channel, uint32_t val)
{
    TC1->TC_CHANNEL[channel].TC_RC = val;
}
void TimerOne::setRA(timerChannel_t channel, uint32_t val)
{
    TC1->TC_CHANNEL[channel].TC_RA = val;
}
void TimerOne::setRB(timerChannel_t channel, uint32_t val)
{
    TC1->TC_CHANNEL[channel].TC_RB = val;
}

void TimerOne::setPeriod(timerChannel_t channel, unsigned long microseconds)
{
    /* We're running off MCK/2 (from TC_CMR_TCCLKS_TIMER_CLOCK1) */
    unsigned long cycles = SystemCoreClock / 1000000L * microseconds / 2 ;
    
    TC1->TC_CHANNEL[channel].TC_RC = cycles;
  /* 
  long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
  
  oldSREG = SREG;				
  cli();							// Disable interrupts for 16 bit register access
  ICR1 = pwmPeriod = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;
  
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock */
}

void TimerOne::attachInterrupt(timerChannel_t channel, TimerOneCallback_t isr)
{
    isrCallbacks[channel] = isr;
    TC1->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS; // Enable RC Compare interrupt
    switch(channel) {
        case CHANNEL0:
            NVIC_SetPriority(TC3_IRQn, 7);
            NVIC_EnableIRQ(TC3_IRQn);
            break;
        case CHANNEL1:
            NVIC_SetPriority(TC4_IRQn, 7);
            NVIC_EnableIRQ(TC4_IRQn);
            break;
        case CHANNEL2:
            NVIC_SetPriority(TC5_IRQn, 7);
            NVIC_EnableIRQ(TC5_IRQn);
            break;
        default:
            break;
    }
}

void TimerOne::detachInterrupt(timerChannel_t channel)
{
    switch(channel) {
        case CHANNEL0:
            NVIC_DisableIRQ(TC3_IRQn);
            break;
        case CHANNEL1:
            NVIC_DisableIRQ(TC4_IRQn);
            break;
        case CHANNEL2:
            NVIC_DisableIRQ(TC5_IRQn);
            break;
        default:
            break;
    }
    stop(channel);
    TC1->TC_CHANNEL[channel].TC_IDR = TC_IDR_CPCS; // Disable RC Compare interrupt
    isrCallbacks[channel] = NULL;
}

void TimerOne::resume(timerChannel_t channel)
{
    TC1->TC_CHANNEL[channel].TC_CCR = TC_CCR_CLKEN; // Enable the clock
}

void TimerOne::start(timerChannel_t channel)	// AR addition, renamed by Lex to reflect it's actual role
{
    TC1->TC_CHANNEL[channel].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void TimerOne::stop(timerChannel_t channel)
{
    TC1->TC_CHANNEL[channel].TC_CCR = TC_CCR_CLKDIS;
}

/* 
 * Synchronize all timer channels
 */
void TimerOne::sync() {
    TC1->TC_BCR = TC_BCR_SYNC; // Set the timer SYNC bit
}

unsigned long TimerOne::read(timerChannel_t channel)		//returns the value of the timer in microseconds
{   
    unsigned long cycles = TC1->TC_CHANNEL[channel].TC_CV;
    return (cycles * 2)/(SystemCoreClock / 1000000L);
    
/*    
									//rember! phase and freq correct mode counts up to then down again
   	unsigned long tmp;				// AR amended to hold more than 65536 (could be nearly double this)
  	unsigned int tcnt1;				// AR added

	oldSREG= SREG;
  	cli();							
  	tmp=TCNT1;    					
	SREG = oldSREG;

	char scale=0;
	switch (clockSelectBits)
	{
	case 1:// no prescalse
		scale=0;
		break;
	case 2:// x8 prescale
		scale=3;
		break;
	case 3:// x64
		scale=6;
		break;
	case 4:// x256
		scale=8;
		break;
	case 5:// x1024
		scale=10;
		break;
	}
	
	do {	// Nothing -- max delay here is ~1023 cycles.  AR modified
		oldSREG = SREG;
		cli();
		tcnt1 = TCNT1;
		SREG = oldSREG;
	} while (tcnt1==tmp); //if the timer has not ticked yet

	//if we are counting down add the top value to how far we have counted down
	tmp = (  (tcnt1>tmp) ? (tmp) : (long)(ICR1-tcnt1)+(long)ICR1  );		// AR amended to add casts and reuse previous TCNT1
	return ((tmp*1000L)/(F_CPU /1000L))<<scale; */
}


#ifdef __cplusplus
 extern "C" {
#endif
     
// Channel 0 ISR
void TC3_IRQHandler(void)
{
    uint32_t status = TC1->TC_CHANNEL[0].TC_SR;
    if (Timer1.isrCallbacks[0] != NULL) {
        (Timer1.isrCallbacks[0])(status);
    }
}

// Channel 1 ISR
void TC4_IRQHandler(void)
{
    uint32_t status = TC1->TC_CHANNEL[1].TC_SR;
    if (Timer1.isrCallbacks[1] != NULL) {
        (Timer1.isrCallbacks[1])(status);
    }
}

// Channel 2 ISR
void TC5_IRQHandler(void)
{
    uint32_t status = TC1->TC_CHANNEL[2].TC_SR;
    if (Timer1.isrCallbacks[2] != NULL) {
        (Timer1.isrCallbacks[2])(status);
    }
}

#ifdef __cplusplus
}
#endif


#endif
