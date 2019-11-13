/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_ARDUINO_DUE_X_
#define _VARIANT_ARDUINO_DUE_X_

#include <stdint.h>
#include <sam3xa.h>


#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#define PINS_COUNT           (79u)
#define NUM_DIGITAL_PINS     (54u)
#define NUM_ANALOG_INPUTS    (12u)

#define digitalPinToPort(P)        ( g_APinDescription[P].pPort )
#define digitalPinToBitMask(P)     ( g_APinDescription[P].ulPin )
#define pinToPort(P)            ( g_APinDescription[P].pPort )
#define pinToBitMask(P)         ( g_APinDescription[P].ulPin )
#define pinToPortID(P)          ( g_APinDescription[P].ulPeripheralId )
#define pinToAnalogChannel(P)   ( g_APinDescription[P].ulADCChannelNumber )
//#define analogInPinToBit(P)        ( )
#define portOutputRegister(port)   ( &(port->PIO_ODSR) )
#define portInputRegister(port)    ( &(port->PIO_PDSR) )
#define digitalPinHasPWM(P)        ( g_APinDescription[P].ulPWMChannel != NOT_ON_PWM || g_APinDescription[P].ulTCChannel != NOT_ON_TIMER )


typedef enum _EAnalogChannel
{
  NO_ADC=-1,
  ADC0=0,
  ADC1,
  ADC2,
  ADC3,
  ADC4,
  ADC5,
  ADC6,
  ADC7,
  ADC8,
  ADC9,
  ADC10,
  ADC11,
  ADC12,
  ADC13,
  ADC14,
  ADC15,
  DA0,
  DA1
} EAnalogChannel;

typedef struct _PinDescription
{
    Pio* pPort;
    uint32_t ulPin;
    uint32_t ulPeripheralId;
    EAnalogChannel ulADCChannelNumber; /* ADC Channel number in the SAM device */
} PinDescription;

/* Pins table to be instanciated into variant.cpp */
extern const PinDescription g_APinDescription[] ;
extern const int g_PinDescriptionLength;

#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* _VARIANT_ARDUINO_DUE_X_ */

