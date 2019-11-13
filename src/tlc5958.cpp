#ifndef _TLC5958_cpp
#define _TLC5958_cpp

#include "tlc5958.h"
#include "wiring_digital.h"
#include <stdlib.h>
#include <string.h> // for memcpy()

arduinoPin_t SCLK_PIN = 16;
arduinoPin_t LAT_PIN = 15;
arduinoPin_t SIN_PIN = 14;

TLC5958::TLC5958(uint32_t len)
{
    if (len == 0) len = 1;
    chainLength = len;
    
    GS = (GS_t*)calloc(len, sizeof(GS_t));
    FC1 = (FC1_t*)calloc(len, sizeof(FC1_t));
    FC2 = (FC2_t*)calloc(len, sizeof(FC2_t));
    
    /* Default power-on values */
    
    FC1[0].LODVTH =    FC1_LODVTH_VLOD1;
    FC1[0].SEL_TD0 =   FC1_SEL_TD0_1;
    FC1[0].SEL_GDLY =  FC1_SEL_GDLY_ENABLE;
    FC1[0].LGSE1B =    FC1_LGSE1B_NONE;
    FC1[0].LGSE1G =    FC1_LGSE1G_NONE;
    FC1[0].LGSE1R =    FC1_LGSE1R_NONE;
    FC1[0].CCB =       0x100u;
    FC1[0].CCG =       0x100u;
    FC1[0].CCR =       0x100u;
    FC1[0].BC =        0x4u;
    FC1[0].WRTFC =     WRTFC_FC1;
    
    FC2[0].MAX_LINE =      0;
    FC2[0].PSAVE_ENA =     FC2_PSAVE_ENA_DISABLE;
    FC2[0].SEL_GCLK_EDGE = FC2_SEL_GCLK_EDGE_RISING;
    FC2[0].SEL_PCHG =      FC2_SEL_PCHG_DISABLE;
    FC2[0].EMI_REDUCE_B =  FC2_EMI_REDUCE_B_DISABLE;
    FC2[0].EMI_REDUCE_G =  FC2_EMI_REDUCE_G_DISABLE;
    FC2[0].EMI_REDUCE_R =  FC2_EMI_REDUCE_R_DISABLE;
    FC2[0].SEL_PWM =       FC2_SEL_PWM_8P8;
    FC2[0].LGSE2 =         FC2_LGSE2_NONE;
    FC2[0].WRTFC =         WRTFC_FC2;
    
    for (int i = 1; i < chainLength; i++) {
        memcpy(&FC1[i], &FC1[0], sizeof(FC1_t));
        memcpy(&FC2[i], &FC2[0], sizeof(FC2_t));
    }
    
    pinMode_arduino(SCLK_PIN, OUTPUT);
    pinMode_arduino(LAT_PIN, OUTPUT);
    pinMode_arduino(SIN_PIN, OUTPUT);
    
    writeLock = false;
}

void TLC5958::replicateFC1() {
    for (int i = 1; i < chainLength; i++) {
        memcpy(&FC1[i], &FC1[0], sizeof(FC1_t));
    }
}
void TLC5958::replicateFC2() {
    for (int i = 1; i < chainLength; i++) {
        memcpy(&FC2[i], &FC2[0], sizeof(FC2_t));
    }
}

void TLC5958::ShiftOutMSB(void* buff, uint8_t pos, uint8_t len)
{
    uint64_t val = *(uint64_t*)buff;
    for (int i = pos-1; i >= pos-len; i--)  {
        digitalWrite_arduino(SIN_PIN, !!((val >> i) & 1));
        digitalWrite_arduino(SCLK_PIN, HIGH);
        digitalWrite_arduino(SCLK_PIN, LOW);        
    }
} 

void TLC5958::WriteBufferCMD(void* buff, uint8_t cmd)
{
    for (int i = chainLength-1; i >= 0; i--)
    /* Transmit data for each chip in the chain */
    {
        if (cmd == CMD_WRTGS)
        {
            buff = &GS[i];
        }
        else if (cmd == CMD_WRTFC && ((FC1_t*)buff)->WRTFC == WRTFC_FC1)
        {
            buff = &FC1[i];
        }
        else if (cmd == CMD_WRTFC && ((FC2_t*)buff)->WRTFC == WRTFC_FC2)
        {
            buff = &FC2[i];
        }
        ShiftOutMSB(
            buff, 
            48,
            i == 0 ? 48 - cmd : 48 // Write all 48 bits if this isn't the last chip in the chain
        );
    }
    digitalWrite_arduino(LAT_PIN, HIGH);
    ShiftOutMSB(buff, cmd, cmd); // Write the last few command bits
    digitalWrite_arduino(LAT_PIN, LOW);
}

void TLC5958::WriteGS() {
    writeLock = true;
    WriteBufferCMD(GS, CMD_WRTGS);
    writeLock = false;
}

void TLC5958::WriteVSYNC() {
    uint64_t temp_buff = 0;
    writeLock = true;
    digitalWrite_arduino(LAT_PIN, HIGH);
    ShiftOutMSB(&temp_buff, CMD_VSYNC, CMD_VSYNC);
    digitalWrite_arduino(LAT_PIN, LOW);
    writeLock = false;
}

void TLC5958::WriteFCWRTEN() {
    uint64_t temp_buff = 0;
    digitalWrite_arduino(LAT_PIN, HIGH);
    ShiftOutMSB(&temp_buff, CMD_FCWRTEN, CMD_FCWRTEN);
    digitalWrite_arduino(LAT_PIN, LOW);
}

void TLC5958::WriteFC1() {
    writeLock = true;
    WriteFCWRTEN();
    WriteBufferCMD(FC1, CMD_WRTFC);
    writeLock = false;
}

void TLC5958::WriteFC2() {
    writeLock = true;
    WriteFCWRTEN();
    WriteBufferCMD(FC2, CMD_WRTFC);
    writeLock = false;
}

bool TLC5958::isWriting() {
    return writeLock;
}

#endif /* _TLC5958_cpp */
