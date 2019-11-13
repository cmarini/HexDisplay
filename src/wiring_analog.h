
#ifndef _WIRING_ANALOG_h
#define _WIRING_ANALOG_h

#include <sam3xa.h>
#include <stdint.h>
#include <stddef.h>
#include "variant.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The max adc sample freq definition*/
#define ADC_FREQ_MAX   20000000
/* The min adc sample freq definition*/
#define ADC_FREQ_MIN    1000000
/* The normal adc startup time*/
#define ADC_STARTUP_NORM     40
/* The fast adc startup time*/
#define ADC_STARTUP_FAST     12
    
/* Arduino analog pin mapping */
typedef enum {
    A0  = 54,
    A1  = 55,
    A2  = 56,
    A3  = 57,
    A4  = 58,
    A5  = 59,
    A6  = 60,
    A7  = 61,
    A8  = 62,
    A9  = 63,
    A10 = 64,
    A11 = 65,
    DAC0 = 66,
    DAC1 = 67,
    CANRX = 68,
    CANTX = 69,
} analogPin_t;

uint32_t adc_init(const uint32_t ul_mck, const uint32_t ul_adc_clock, const uint8_t uc_startuptime);
void analogInit(analogPin_t pin);
void analog_startFreerun(void);
void analog_stopFreerun(void);
uint32_t analog_read(analogPin_t pin);
uint32_t analog_readWait(analogPin_t pin);

void analog_read_sequence(uint8_t len, analogPin_t pin_list[], uint32_t results[]);


#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ANALOG_h */
