
#include "wiring_analog.h"
#include "wiring_digital.h"
#include "component/pmc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialize the given ADC with the specified ADC clock and startup time.
 *
 * \param ul_mck Main clock of the device (value in Hz).
 * \param ul_adc_clock Analog-to-Digital conversion clock (value in Hz).
 * \param uc_startup ADC start up time. Please refer to the product datasheet
 * for details.
 *
 * \return 0 on success.
 */
uint32_t adc_init(const uint32_t ul_mck, const uint32_t ul_adc_clock, const uint8_t uc_startuptime)
{
	uint32_t startup_table[] = { 0, 8, 16, 24, 64, 80, 96, 112, 512, 576, 640, 704, 768, 832, 896, 960 };
	uint32_t ul_prescal, ul_startup,  ul_mr_startup, ul_real_adc_clock;
	ADC->ADC_CR = ADC_CR_SWRST;

	/* Reset Mode Register. */
	ADC->ADC_MR = 0;

	/* Reset PDC transfer. */
	ADC->ADC_PTCR = (ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS);
	ADC->ADC_RCR = 0;
	ADC->ADC_RNCR = 0;
	if (ul_mck % (2 * ul_adc_clock)) {
		// Division with reminder
		ul_prescal = ul_mck / (2 * ul_adc_clock);
	} else {
		// Whole division
		ul_prescal = ul_mck / (2 * ul_adc_clock) - 1;
	}
	ul_real_adc_clock = ul_mck / (2 * (ul_prescal + 1));

	// ADC clocks needed to get ul_startuptime uS
	ul_startup = (ul_real_adc_clock / 1000000) * uc_startuptime;

	// Find correct MR_STARTUP value from conversion table
	for (ul_mr_startup=0; ul_mr_startup<16; ul_mr_startup++) {
		if (startup_table[ul_mr_startup] >= ul_startup)
			break;
	}
	if (ul_mr_startup==16)
		return 1;
	ADC->ADC_MR |= ADC_MR_PRESCAL(ul_prescal) |
			((ul_mr_startup << ADC_MR_STARTUP_Pos) & ADC_MR_STARTUP_Msk);
	return 0;
}
    
void analogInit(analogPin_t pin)
{
    pinMode(pinToPort(pin), pinToBitMask(pin), PERIPHERAL_B);
    unsigned int channel = pinToAnalogChannel(pin);
	switch ( channel )
	{
		case ADC0 :
		case ADC1 :
		case ADC2 :
		case ADC3 :
		case ADC4 :
		case ADC5 :
		case ADC6 :
		case ADC7 :
		case ADC8 :
		case ADC9 :
		case ADC10 :
		case ADC11 :

			// Enable the corresponding channel
			if ((ADC->ADC_CHSR & (1 << channel)) != 1) {
                ADC->ADC_CHER = 1 << channel;
			}

			// Start the ADC
//			ADC->ADC_CR = ADC_CR_START;
			break;

		// Compiler could yell because we don't handle DAC pins
		default :
			break;
	}
    
}

void analog_startFreerun()
{
    ADC->ADC_MR |= ADC_MR_FREERUN;
}
void analog_stopFreerun()
{
    ADC->ADC_MR &= ~ADC_MR_FREERUN;
}

uint32_t analog_read(analogPin_t pin)
{
    return ADC->ADC_CDR[pinToAnalogChannel(pin)];
}

uint32_t analog_readWait(analogPin_t pin)
{
    ADC->ADC_CHDR = 0xFFFFu;
    ADC->ADC_CHER = 1 << pinToAnalogChannel(pin);
    ADC->ADC_CR = ADC_CR_START;
    while (!(ADC->ADC_ISR & (1 << pinToAnalogChannel(pin))));
    ADC->ADC_CHDR = 1 << pinToAnalogChannel(pin);
    return ADC->ADC_CDR[pinToAnalogChannel(pin)];
}

void analog_read_sequence(uint8_t len, analogPin_t pin_list[], uint32_t results[])
{
	uint8_t i;
    for (i = 0; i < 8; i++) {
        ADC->ADC_SEQR1 |= pinToAnalogChannel(pin_list[i]) << (4 * i);
    }
    if (len > 8) {
		for (i = 0; i < len - 8; i++) {
			ADC->ADC_SEQR2 |= pinToAnalogChannel(pin_list[i]) << (4 * i);
		}
	}
    
    uint32_t prev_CHSR = ADC->ADC_CHSR; // Save the currently enabled/disabled channels
    ADC->ADC_CHDR = 0xFFFFu; // Disable all channels;
    for (i = 0; i < len; i++) {
        ADC->ADC_CHER = 1 << pinToAnalogChannel(pin_list[i]); // Enable channels in this sequence
//        ADC->ADC_CDR[pinToAnalogChannel(pin_list[i])]; // Clear 
    }
    
    ADC->ADC_MR |= ADC_MR_USEQ_REG_ORDER; // Use defined sequence
    
    ADC->ADC_CR = ADC_CR_START; // Start the conversion
    
    // Wait for each channel to finish conversion before getting the result
    for (i = 0; i < len; i++) {
        while (!(ADC->ADC_ISR & (1 << pinToAnalogChannel(pin_list[i]))));
        results[i] = ADC->ADC_CDR[pinToAnalogChannel(pin_list[i])];
    }
    
    ADC->ADC_MR &= ~ADC_MR_USEQ_REG_ORDER; // Disable sequencer order
    ADC->ADC_CHDR = ~prev_CHSR; // Disabled previously disabled channels
    ADC->ADC_CHER = prev_CHSR; // Enabled previously enabled channels
}


#ifdef __cplusplus
}
#endif

