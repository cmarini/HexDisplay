
#include "wiring_digital.h"
#include "component/pmc.h"


callback_t PIO_isrCallbacks[4][32];


void pinMode( Pio* port, uint32_t ulPinMask, uint32_t ulMode )
{
    if (ulMode == INPUT || ulMode == INPUT_PULLUP)
    {
        pmc_set_writeprotect(0); // disable PMC write protection
        pmc_enable_periph_clk(portToPeriphID(port));
        port->PIO_ODR = ulPinMask; // disable output mode
        port->PIO_IFDR = ulPinMask; // disable glitch/debounce filter
        if (ulMode == INPUT_PULLUP)
        {
            port->PIO_PUER = ulPinMask; // enable pull-up resistor
        }
        else
        {
            port->PIO_PUDR = ulPinMask; // disable pull-up resistor
        }
    } 
    else if (ulMode == OUTPUT)
    {
        port->PIO_CODR = ulPinMask; // set output low
        port->PIO_OER = ulPinMask;  // enable output
    
        /* if all pins are output and no interrupts are enabled, 
           disable PIO Controller clocking, reduce power consumption */
        if ((port->PIO_OSR == 0xffffffff) && (port->PIO_IMR == 0))
        {
            pmc_disable_periph_clk(portToPeriphID(port));
        }
    }
    
    if (ulMode == PERIPHERAL_A)
    {
        port->PIO_ABSR &= ~ulPinMask; // select peripheral A
        port->PIO_PDR = ulPinMask; // disable PIO control
    }
    else if (ulMode == PERIPHERAL_B)
    {
        port->PIO_ABSR |= ulPinMask; // select peripheral B
        port->PIO_PDR = ulPinMask; // disable PIO control
    }
    else
    {
        port->PIO_PER = ulPinMask; // enable PIO control
    }

}

void pinMode_arduino( arduinoPin_t ulPin, uint32_t ulMode )
{
    pinMode(pinToPort(ulPin), pinToBitMask(ulPin), ulMode);
}

void setDebounce( Pio* port, uint32_t ulPinMask, uint32_t ulMillis )
{
    if (ulMillis == 0)
    { // use glitch filter
        port->PIO_SCIFSR = ulPinMask; // select glitch filter
    }
    else
    { // use debounce filter
        if (ulMillis > 1000) ulMillis = 1000;
        port->PIO_SCDR = ((32768 * ulMillis / 2000) - 1) & 0x3FFF; // set slow clock divider
        port->PIO_DIFSR = ulPinMask; // select debounce filter
    }
    port->PIO_IFER = ulPinMask; // enable glitch/debounce filter
        
}
void setDebounce_arduino( arduinoPin_t ulPin, uint32_t ulMillis )
{
    setDebounce(pinToPort(ulPin), pinToBitMask(ulPin), ulMillis);
}

void digitalWriteSync( Pio* port, uint32_t ulPinMask, uint32_t ulVal )
{
    port->PIO_OWDR = 0xFFFFFFFFu;   // Clear the output write mask
    port->PIO_OWER = ulPinMask;     // Set the output write mask
    port->PIO_ODSR = ulVal;         // Write the output values
}


void digitalWrite( Pio* port, uint32_t ulPinMask, uint32_t ulVal )
{
    if (ulVal)
    {
        port->PIO_SODR = ulPinMask;
    }
    else
    {
        port->PIO_CODR = ulPinMask ;
    }
}

void digitalWrite_arduino( arduinoPin_t ulPin, uint32_t ulVal )
{
//    static arduinoPin_t blah;
//    blah = ulPin;
//    PinDescription pd = g_APinDescription[ulPin];
//    Pio* port = pd.pPort;
//    uint32_t bm = pd.ulPin;
//    digitalWrite(port, bm, ulVal);
    if (ulPin >= g_PinDescriptionLength) {
        return;
    }
    digitalWrite(pinToPort(ulPin), pinToBitMask(ulPin), ulVal);
}

uint32_t digitalRead( Pio* port, uint32_t ulPinMask )
{
    uint32_t pioOutPins = ulPinMask &
                          port->PIO_OSR &   // pins configured as an output
                          port->PIO_PSR ;   // pins configured for PIO control
    
    return (
        (port->PIO_ODSR & pioOutPins) | // read PIO output pins as their set output data values
        (port->PIO_PDSR & (~pioOutPins & ulPinMask))  // read all other pins from their actual statuses
    );
}
uint32_t digitalRead_arduino( arduinoPin_t ulPin )
{
    return digitalRead(pinToPort(ulPin), pinToBitMask(ulPin)) ? HIGH : LOW;
}

void attachInterrupt( Pio* port, uint32_t ulPinMask, uint32_t ulMode, callback_t isr)
{
    pmc_set_writeprotect(0); // disable PMC write protection
    pmc_enable_periph_clk(portToPeriphID(port));
    
    switch (ulMode)
    {
        case LOW:
            port->PIO_AIMER = ulPinMask;    // enable additional interrupt modes
            port->PIO_LSR = ulPinMask;      // enable level-detect interrupt mode
            port->PIO_FELLSR = ulPinMask;   // set low level detection
            break;
        case HIGH:
            port->PIO_AIMER = ulPinMask;    // enable additional interrupt modes
            port->PIO_LSR = ulPinMask;      // enable level-detect interrupt mode
            port->PIO_REHLSR = ulPinMask;   // set high level detection
            break;
        case CHANGE:
            port->PIO_AIMDR = ulPinMask;    // disable additional interrupt modes
        case FALLING:
            port->PIO_AIMER = ulPinMask;    // enable additional interrupt modes
            port->PIO_ESR = ulPinMask;      // enable edge-detect interrupt mode
            port->PIO_FELLSR = ulPinMask;   // set falling edge detection
            break;
        case RISING:
            port->PIO_AIMER = ulPinMask;    // enable additional interrupt modes
            port->PIO_ESR = ulPinMask;      // enable edge-detect interrupt mode
            port->PIO_REHLSR = ulPinMask;   // set rising edge detection
            break;
        default:
            port->PIO_IDR = ulPinMask;      // disable interrupts
            return; // skip enabling the interrupts
    }
    
    int portID = portToPeriphID(port);
    int p = portID - ID_PIOA;
    for (int i = 0; i < 32; i++) 
    {
        if (ulPinMask & (1 << i)) 
        {
            PIO_isrCallbacks[p][i] = isr;
        }
    }
    port->PIO_ISR; // clear Interrupt Status Register
    port->PIO_IER = ulPinMask; // enable interrupts
    
//    NVIC_DisableIRQ((IRQn_Type)portID);
//    NVIC_ClearPendingIRQ((IRQn_Type)portID);
    NVIC_EnableIRQ((IRQn_Type)portID);
}

void attachInterrupt_arduino( arduinoPin_t ulPin, uint32_t ulMode, callback_t isr )
{
    attachInterrupt(pinToPort(ulPin), pinToBitMask(ulPin), ulMode, isr);
}

void detachInterrupt( Pio* port, uint32_t ulPinMask )
{
    port->PIO_IDR = ulPinMask; // disable interrupts
    
    int p = portToPeriphID(port) - ID_PIOA;
    for (int i = 0; i < 32; i++) 
    {
        if (ulPinMask & (1 << i)) 
        {
            PIO_isrCallbacks[p][i] = NULL;
        }
    }
}

void detachInterrupt_arduino( arduinoPin_t ulPin )
{
    detachInterrupt(pinToPort(ulPin), pinToBitMask(ulPin));
}

uint32_t portToPeriphID(Pio* port)
{
    if (port == PIOA) return ID_PIOA;
    if (port == PIOB) return ID_PIOB;
    if (port == PIOC) return ID_PIOC;
    if (port == PIOD) return ID_PIOD;
    return ID_PERIPH_COUNT; // Invalid Peripheral ID
}


#ifdef __cplusplus
extern "C" {
#endif
  
extern void PIOA_IRQHandler ( void )
{
    uint32_t status = PIOA->PIO_ISR;
    for (int i = 0; i < 32; i++) 
    {
        if (status & (1 << i)) 
        {
            if (PIO_isrCallbacks[0][i] != NULL) 
            {
                (PIO_isrCallbacks[0][i])();
            }
        }
    }
}
extern void PIOB_IRQHandler ( void )
{
    uint32_t status = PIOB->PIO_ISR;
    for (int i = 0; i < 32; i++) 
    {
        if (status & (1 << i)) 
        {
            if (PIO_isrCallbacks[1][i] != NULL) 
            {
                (PIO_isrCallbacks[1][i])();
            }
        }
    }
}
extern void PIOC_IRQHandler ( void )
{
    uint32_t status = PIOC->PIO_ISR;
    for (int i = 0; i < 32; i++) 
    {
        if (status & (1 << i)) 
        {
            if (PIO_isrCallbacks[2][i] != NULL) 
            {
                (PIO_isrCallbacks[2][i])();
            }
        }
    }
}
extern void PIOD_IRQHandler ( void )
{
    uint32_t status = PIOD->PIO_ISR;
    for (int i = 0; i < 32; i++) 
    {
        if (status & (1 << i)) 
        {
            if (PIO_isrCallbacks[3][i] != NULL) 
            {
                (PIO_isrCallbacks[3][i])();
            }
        }
    }
}

#ifdef __cplusplus
}
#endif

