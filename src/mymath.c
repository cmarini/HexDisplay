
#include <sam3xa.h>
#include "mymath.h"
#include "component/pmc.h"

/*
long map(long long x, long long in_min, long long in_max, long long out_min, long long out_max)
{
    if (in_max == in_min) // Make sure we don't divide by zero
    {
        return in_min;
    }
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
*/

void randomInit() {
    pmc_set_writeprotect(0); // disable PMC write protection
    pmc_enable_periph_clk(ID_TRNG);  // enable clock for TRNG
    TRNG->TRNG_CR = TRNG_CR_ENABLE | TRNG_CR_KEY(0x524e47u); // Enable random number generation
}
    

uint32_t getTRNG() {
    while (!(TRNG->TRNG_ISR & TRNG_ISR_DATRDY)); // Wait for the number to be generated
    return TRNG->TRNG_ODATA;
}

long random( long howbig )
{
  if ( howbig == 0 )
  {
    return 0;
  }

  return getTRNG() % howbig;
}

long randomRange( long howsmall, long howbig )
{
  if (howsmall >= howbig)
  {
    return howsmall;
  }

  return random(howbig - howsmall) + howsmall;
}

