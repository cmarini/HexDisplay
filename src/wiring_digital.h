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

#ifndef _WIRING_DIGITAL_h
#define _WIRING_DIGITAL_h

#include <stdint.h>
#include <stddef.h>
#include "variant.h"

#define HIGH 0x1
#define LOW  0x0

//#define INPUT 0x0
//#define OUTPUT 0x1
//#define INPUT_PULLUP 0x2

#define OUTPUT          0
#define INPUT           1
#define INPUT_PULLUP    2
#define PERIPHERAL_A    3
#define PERIPHERAL_B    4

//      LOW 0
//      HIGH 1
#define CHANGE 2
#define FALLING 3
#define RISING 4

#define DEFAULT 1
#define EXTERNAL 0

#ifdef __cplusplus
 extern "C" {
#endif

typedef void (* callback_t)();
typedef uint32_t arduinoPin_t;

     
/**
 * \brief Configures the specified pin to behave either as an input or an output. See the description of digital pins for details.
 *
 * \param ulPin The number of the pin whose mode you wish to set
 * \param ulMode Either INPUT or OUTPUT
 */
extern void pinMode( Pio* port, uint32_t ulPinMask, uint32_t ulMode );
extern void pinMode_arduino( arduinoPin_t ulPin, uint32_t dwMode ) ;

extern void setDebounce( Pio* port, uint32_t ulPinMask, uint32_t ulMillis );
extern void setDebounce_arduino( arduinoPin_t ulPin, uint32_t ulMillis );
/**
 * \brief Write a HIGH or a LOW value to a digital pin.
 *
 * If the pin has been configured as an OUTPUT with pinMode(), its voltage will be set to the
 * corresponding value: 5V (or 3.3V on 3.3V boards) for HIGH, 0V (ground) for LOW.
 *
 * If the pin is configured as an INPUT, writing a HIGH value with digitalWrite() will enable an internal
 * 20K pullup resistor (see the tutorial on digital pins). Writing LOW will disable the pullup. The pullup
 * resistor is enough to light an LED dimly, so if LEDs appear to work, but very dimly, this is a likely
 * cause. The remedy is to set the pin to an output with the pinMode() function.
 *
 * \note Digital pin PIN_LED is harder to use as a digital input than the other digital pins because it has an LED
 * and resistor attached to it that's soldered to the board on most boards. If you enable its internal 20k pull-up
 * resistor, it will hang at around 1.7 V instead of the expected 5V because the onboard LED and series resistor
 * pull the voltage level down, meaning it always returns LOW. If you must use pin PIN_LED as a digital input, use an
 * external pull down resistor.
 *
 * \param dwPin the pin number
 * \param dwVal HIGH or LOW
 */
extern void digitalWriteSync( Pio* port, uint32_t ulPinMask, uint32_t ulVal );
extern void digitalWrite( Pio* port, uint32_t ulPinMask, uint32_t ulVal );
extern void digitalWrite_arduino( arduinoPin_t ulPin, uint32_t dwVal ) ;

/**
 * \brief Reads the value from a specified digital pin, either HIGH or LOW.
 *
 * \param ulPin The number of the digital pin you want to read (int)
 *
 * \return HIGH or LOW
 */
extern uint32_t digitalRead( Pio* port, uint32_t ulPin ) ;
extern uint32_t digitalRead_arduino( arduinoPin_t ulPin ) ;


extern void attachInterrupt( Pio* port, uint32_t ulPinMask, uint32_t ulMode, callback_t isr );
extern void attachInterrupt_arduino( arduinoPin_t ulPin, uint32_t ulMode, callback_t isr );

extern void detachInterrupt( Pio* port, uint32_t ulPinMask );
extern void detachInterrupt_arduino( arduinoPin_t ulPin );

#ifdef __cplusplus
}
#endif


uint32_t portToPeriphID(Pio* port);

#endif /* _WIRING_DIGITAL_h */
