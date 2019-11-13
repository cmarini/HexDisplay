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

#include "variant.h"

/*
 * DUE Board pin   |  PORT  | Label
 * ----------------+--------+-------
 *   0             |  PA8   | "RX0"
 *   1             |  PA9   | "TX0"
 *   2       TIOA0 |  PB25  |
 *   3       TIOA7 |  PC28  |
 *   4       NPCS1 |  PA29  |
 *           TIOB6 |  PC26  |
 *   5       TIOA6 |  PC25  |
 *   6       PWML7 |  PC24  |
 *   7       PWML6 |  PC23  |
 *   8       PWML5 |  PC22  |
 *   9       PWML4 |  PC21  |
 *  10       NPCS0 |  PA28  |
 *           TIOB7 |  PC29  |
 *  11       TIOA8 |  PD7   |
 *  12       TIOB8 |  PD8   |
 *  13       TIOB0 |  PB27  | LED AMBER "L"
 *  14       TXD3  |  PD4   | "TX3"
 *  15       RXD3  |  PD5   | "RX3"
 *  16       TXD1  |  PA13  | "TX2"
 *  17       RXD1  |  PA12  | "RX2"
 *  18       TXD0  |  PA11  | "TX1"
 *  19       RXD0  |  PA10  | "RX1"
 *  20             |  PB12  | "SDA"
 *  21             |  PB13  | "SCL"
 *  22             |  PB26  |
 *  23             |  PA14  |
 *  24             |  PA15  |
 *  25             |  PD0   |
 *  26             |  PD1   |
 *  27             |  PD2   |
 *  28             |  PD3   |
 *  29             |  PD6   |
 *  30             |  PD9   |
 *  31             |  PA7   |
 *  32             |  PD10  |
 *  33             |  PC1   |
 *  34             |  PC2   |
 *  35             |  PC3   |
 *  36             |  PC4   |
 *  37             |  PC5   |
 *  38             |  PC6   |
 *  39             |  PC7   |
 *  40             |  PC8   |
 *  41             |  PC9   |
 *  42             |  PA19  |
 *  43             |  PA20  |
 *  44             |  PC19  |
 *  45             |  PC18  |
 *  46             |  PC17  |
 *  47             |  PC16  |
 *  48             |  PC15  |
 *  49             |  PC14  |
 *  50             |  PC13  |
 *  51             |  PC12  |
 *  52       NPCS2 |  PB21  |
 *  53             |  PB14  |
 *  54             |  PA16  | "A0"
 *  55             |  PA24  | "A1"
 *  56             |  PA23  | "A2"
 *  57             |  PA22  | "A3"
 *  58       TIOB2 |  PA6   | "A4"
 *  69             |  PA4   | "A5"
 *  60       TIOB1 |  PA3   | "A6"
 *  61       TIOA1 |  PA2   | "A7"
 *  62             |  PB17  | "A8"
 *  63             |  PB18  | "A9"
 *  64             |  PB19  | "A10"
 *  65             |  PB20  | "A11"
 *  66             |  PB15  | "DAC0"
 *  67             |  PB16  | "DAC1"
 *  68             |  PA1   | "CANRX"
 *  69             |  PA0   | "CANTX"
 *  70             |  PA17  | "SDA1"
 *  71             |  PA18  | "SCL1"
 *  72             |  PC30  | LED AMBER "RX"
 *  73             |  PA21  | LED AMBER "TX"
 *  74       MISO  |  PA25  |
 *  75       MOSI  |  PA26  |
 *  76       SCLK  |  PA27  |
 *  77       NPCS0 |  PA28  |
 *  78       NPCS3 |  PB23  | unconnected!
 *
 * USB pin         |  PORT
 * ----------------+--------
 *  ID             |  PB11
 *  VBOF           |  PB10
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pins descriptions
 */
extern const PinDescription g_APinDescription[]=
{
  // 0 .. 53 - Digital pins
  // ----------------------
  // 0/1 - UART (Serial)
  { PIOA, PIO_PA8A_URXD,        ID_PIOA, NO_ADC }, // URXD
  { PIOA, PIO_PA9A_UTXD,        ID_PIOA, NO_ADC }, // UTXD

  // 2
  { PIOB, PIO_PB25B_TIOA0,      ID_PIOB, NO_ADC }, // TIOA0
  { PIOC, PIO_PC28B_TIOA7,      ID_PIOC, NO_ADC }, // TIOA7
  { PIOC, PIO_PC26B_TIOB6,      ID_PIOC, NO_ADC }, // TIOB6

  // 5
  { PIOC, PIO_PC25B_TIOA6,      ID_PIOC, NO_ADC }, // TIOA6
  { PIOC, PIO_PC24B_PWML7,      ID_PIOC, NO_ADC }, // PWML7
  { PIOC, PIO_PC23B_PWML6,      ID_PIOC, NO_ADC }, // PWML6
  { PIOC, PIO_PC22B_PWML5,      ID_PIOC, NO_ADC }, // PWML5
  { PIOC, PIO_PC21B_PWML4,      ID_PIOC, NO_ADC }, // PWML4
  // 10
  { PIOC, PIO_PC29B_TIOB7,      ID_PIOC, NO_ADC }, // TIOB7
  { PIOD, PIO_PD7B_TIOA8,       ID_PIOD, NO_ADC }, // TIOA8
  { PIOD, PIO_PD8B_TIOB8,       ID_PIOD, NO_ADC }, // TIOB8

  // 13 - AMBER LED
  { PIOB, PIO_PB27B_TIOB0,      ID_PIOB, NO_ADC }, // TIOB0

  // 14/15 - USART3 (Serial3)
  { PIOD, PIO_PD4B_TXD3,        ID_PIOD, NO_ADC }, // TXD3
  { PIOD, PIO_PD5B_RXD3,        ID_PIOD, NO_ADC }, // RXD3

  // 16/17 - USART1 (Serial2)
  { PIOA, PIO_PA13A_TXD1,       ID_PIOA, NO_ADC }, // TXD1
  { PIOA, PIO_PA12A_RXD1,       ID_PIOA, NO_ADC }, // RXD1

  // 18/19 - USART0 (Serial1)
  { PIOA, PIO_PA11A_TXD0,       ID_PIOA, NO_ADC }, // TXD0
  { PIOA, PIO_PA10A_RXD0,       ID_PIOA, NO_ADC }, // RXD0

  // 20/21 - TWI1
  { PIOB, PIO_PB12A_TWD1,       ID_PIOB, NO_ADC }, // TWD1 - SDA0
  { PIOB, PIO_PB13A_TWCK1,      ID_PIOB, NO_ADC }, // TWCK1 - SCL0

  // 22
  { PIOB, PIO_PB26,             ID_PIOB, NO_ADC }, // PIN 22
  { PIOA, PIO_PA14,             ID_PIOA, NO_ADC }, // PIN 23
  { PIOA, PIO_PA15,             ID_PIOA, NO_ADC }, // PIN 24
  { PIOD, PIO_PD0,              ID_PIOD, NO_ADC }, // PIN 25

  // 26
  { PIOD, PIO_PD1,              ID_PIOD, NO_ADC }, // PIN 26
  { PIOD, PIO_PD2,              ID_PIOD, NO_ADC }, // PIN 27
  { PIOD, PIO_PD3,              ID_PIOD, NO_ADC }, // PIN 28
  { PIOD, PIO_PD6,              ID_PIOD, NO_ADC }, // PIN 29

  // 30
  { PIOD, PIO_PD9,              ID_PIOD, NO_ADC }, // PIN 30
  { PIOA, PIO_PA7,              ID_PIOA, NO_ADC }, // PIN 31
  { PIOD, PIO_PD10,             ID_PIOD, NO_ADC }, // PIN 32
  { PIOC, PIO_PC1,              ID_PIOC, NO_ADC }, // PIN 33

  // 34
  { PIOC, PIO_PC2,              ID_PIOC, NO_ADC }, // PIN 34
  { PIOC, PIO_PC3,              ID_PIOC, NO_ADC }, // PIN 35
  { PIOC, PIO_PC4,              ID_PIOC, NO_ADC }, // PIN 36
  { PIOC, PIO_PC5,              ID_PIOC, NO_ADC }, // PIN 37

  // 38
  { PIOC, PIO_PC6,              ID_PIOC, NO_ADC }, // PIN 38
  { PIOC, PIO_PC7,              ID_PIOC, NO_ADC }, // PIN 39
  { PIOC, PIO_PC8,              ID_PIOC, NO_ADC }, // PIN 40
  { PIOC, PIO_PC9,              ID_PIOC, NO_ADC }, // PIN 41

  // 42
  { PIOA, PIO_PA19,             ID_PIOA, NO_ADC }, // PIN 42
  { PIOA, PIO_PA20,             ID_PIOA, NO_ADC }, // PIN 43
  { PIOC, PIO_PC19,             ID_PIOC, NO_ADC }, // PIN 44
  { PIOC, PIO_PC18,             ID_PIOC, NO_ADC }, // PIN 45

  // 46
  { PIOC, PIO_PC17,             ID_PIOC, NO_ADC }, // PIN 46
  { PIOC, PIO_PC16,             ID_PIOC, NO_ADC }, // PIN 47
  { PIOC, PIO_PC15,             ID_PIOC, NO_ADC }, // PIN 48
  { PIOC, PIO_PC14,             ID_PIOC, NO_ADC }, // PIN 49

  // 50
  { PIOC, PIO_PC13,             ID_PIOC, NO_ADC }, // PIN 50
  { PIOC, PIO_PC12,             ID_PIOC, NO_ADC }, // PIN 51
  { PIOB, PIO_PB21,             ID_PIOB, NO_ADC }, // PIN 52
  { PIOB, PIO_PB14,             ID_PIOB, NO_ADC }, // PIN 53


  // 54 .. 65 - Analog pins
  // ----------------------
  { PIOA, PIO_PA16X1_AD7,       ID_PIOA, ADC7   }, // AD0
  { PIOA, PIO_PA24X1_AD6,       ID_PIOA, ADC6   }, // AD1
  { PIOA, PIO_PA23X1_AD5,       ID_PIOA, ADC5   }, // AD2
  { PIOA, PIO_PA22X1_AD4,       ID_PIOA, ADC4   }, // AD3
  // 58
  { PIOA, PIO_PA6X1_AD3,        ID_PIOA, ADC3   }, // AD4
  { PIOA, PIO_PA4X1_AD2,        ID_PIOA, ADC2   }, // AD5
  { PIOA, PIO_PA3X1_AD1,        ID_PIOA, ADC1   }, // AD6
  { PIOA, PIO_PA2X1_AD0,        ID_PIOA, ADC0   }, // AD7
  // 62
  { PIOB, PIO_PB17X1_AD10,      ID_PIOB, ADC10  }, // AD8
  { PIOB, PIO_PB18X1_AD11,      ID_PIOB, ADC11  }, // AD9
  { PIOB, PIO_PB19X1_AD12,      ID_PIOB, ADC12  }, // AD10
  { PIOB, PIO_PB20X1_AD13,      ID_PIOB, ADC13  }, // AD11

  // 66/67 - DAC0/DAC1
  { PIOB, PIO_PB15X1_DAC0,      ID_PIOB, DA0    }, // DAC0
  { PIOB, PIO_PB16X1_DAC1,      ID_PIOB, DA1    }, // DAC1

  // 68/69 - CANRX0/CANTX0
  { PIOA, PIO_PA1A_CANRX0,      ID_PIOA, NO_ADC }, // CANRX
  { PIOA, PIO_PA0A_CANTX0,      ID_PIOA, NO_ADC }, // CANTX

  // 70/71 - TWI0
  { PIOA, PIO_PA17A_TWD0,       ID_PIOA, NO_ADC }, // TWD0 - SDA1
  { PIOA, PIO_PA18A_TWCK0,      ID_PIOA, NO_ADC }, // TWCK0 - SCL1

  // 72/73 - LEDs
  { PIOC, PIO_PC30,             ID_PIOC, NO_ADC }, // LED AMBER RXL
  { PIOA, PIO_PA21,             ID_PIOA, NO_ADC }, // LED AMBER TXL

  // 74/75/76 - SPI
  { PIOA, PIO_PA25A_SPI0_MISO,  ID_PIOA, NO_ADC }, // MISO
  { PIOA, PIO_PA26A_SPI0_MOSI,  ID_PIOA, NO_ADC }, // MOSI
  { PIOA, PIO_PA27A_SPI0_SPCK,  ID_PIOA, NO_ADC }, // SPCK

  // 77 - SPI CS0
  { PIOA, PIO_PA28A_SPI0_NPCS0, ID_PIOA, NO_ADC }, // NPCS0

  // 78 - SPI CS3 (unconnected)
  { PIOB, PIO_PB23B_SPI0_NPCS3, ID_PIOB, NO_ADC }, // NPCS3

  // 79 .. 84 - "All pins" masks

  // 79 - TWI0 all pins
  { PIOA, PIO_PA17A_TWD0|PIO_PA18A_TWCK0, ID_PIOA, NO_ADC },
  // 80 - TWI1 all pins
  { PIOB, PIO_PB12A_TWD1|PIO_PB13A_TWCK1, ID_PIOB, NO_ADC },
  // 81 - UART (Serial) all pins
  { PIOA, PIO_PA8A_URXD|PIO_PA9A_UTXD, ID_PIOA, NO_ADC },
  // 82 - USART0 (Serial1) all pins
  { PIOA, PIO_PA11A_TXD0|PIO_PA10A_RXD0, ID_PIOA, NO_ADC },
  // 83 - USART1 (Serial2) all pins
  { PIOA, PIO_PA13A_TXD1|PIO_PA12A_RXD1, ID_PIOA, NO_ADC },
  // 84 - USART3 (Serial3) all pins
  { PIOD, PIO_PD4B_TXD3|PIO_PD5B_RXD3, ID_PIOD, NO_ADC },

  // 85 - USB
  { PIOB, PIO_PB11A_UOTGID|PIO_PB10A_UOTGVBOF, ID_PIOB, NO_ADC }, // ID - VBOF

  // 86 - SPI CS2
  { PIOB, PIO_PB21B_SPI0_NPCS2, ID_PIOB, NO_ADC }, // NPCS2

  // 87 - SPI CS1
  { PIOA, PIO_PA29A_SPI0_NPCS1, ID_PIOA, NO_ADC }, // NPCS1

  // 88/89 - CANRX1/CANTX1 (same physical pin for 66/53)
  { PIOB, PIO_PB15A_CANRX1,     ID_PIOB, NO_ADC }, // CANRX1
  { PIOB, PIO_PB14A_CANTX1,     ID_PIOB, NO_ADC }, // CANTX1

  // 90 .. 91 - "All CAN pins" masks
  // 90 - CAN0 all pins
  { PIOA, PIO_PA1A_CANRX0|PIO_PA0A_CANTX0, ID_PIOA, NO_ADC },
  // 91 - CAN1 all pins
  { PIOB, PIO_PB15A_CANRX1|PIO_PB14A_CANTX1, ID_PIOB, NO_ADC },

  // END
  { 0, 0, 0 }
} ;

const int g_PinDescriptionLength = sizeof(g_APinDescription)/sizeof(PinDescription);

#ifdef __cplusplus
}
#endif

