
#include "display_tlc5958.h"
#include "display_config.h"

#include "wiring_digital.h"
#include "component/pmc.h"
#include "component/wiring.h"
#include <sam3xa.h>

#if HEX_SIDE_LENGTH == 8
int tlcToPixel_lut[NUM_TLC_ROWS][NUM_TLC_COLS] =
{
    {  0,   1,   2,   3,   4,   5,   6,   7,  16,  91, 105, 118, 130, 141, 151, 160, 168, 167, 161, 152, 142, 131, 119, 106,  92,  77,  63,  70,  84,  -1,  -1,  -1},
    {  8,   9,  10,  11,  12,  13,  14,  15,  25,  76,  90, 104, 117, 129, 140, 150, 159, 158, 162, 153, 143, 132, 120, 107,  93,  78,  64,  85,  -1,  -1,  -1,  -1},
    { 17,  18,  19,  20,  21,  22,  23,  24,  35,  62,  75,  89, 103, 116, 128, 139, 149, 148, 163, 154, 144, 133, 121, 108,  94,  79,  65,  99,  -1,  -1,  -1,  -1},
    { 27,  28,  29,  30,  31,  32,  33,  34,  46,  49,  61,  74,  88, 102, 115, 127, 138, 137, 164, 155, 145, 134, 122, 109,  95,  80,  66,  98,  -1,  -1,  -1,  -1},
    { 38,  39,  40,  41,  42,  43,  44,  45,  58,  37,  48,  60,  73,  87, 101, 114, 126, 125, 165, 156, 146, 135, 123, 110,  96,  81,  67,  83,  -1,  -1,  -1,  -1},
    { 50,  51,  52,  53,  54,  55,  56,  57,  71,  26,  36,  47,  59,  72,  86, 100, 113, 112, 166, 157, 147, 136, 124, 111,  97,  82,  68,  69,  -1,  -1,  -1,  -1},
};
#elif HEX_SIDE_LENGTH == 3
int tlcToPixel_lut[NUM_TLC_ROWS][NUM_TLC_COLS] =
{
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
};
#else
    #error Table not precalculated
#endif

static const int scanLinePinsMask = 0x1FFu << SCAN_LINE_PINS_OFFSET; // Pins PC1 - PC9 (Arduino 33-41)
    
void displayInterface_init()
{
    /*
     *  Set up clocks for display operation.
     *  Generate a waveform on a pin for the TLC5958 GS clock.
     *  Switch display row power in timed interrupts.
     */
    
    pmc_enable_periph_clk(ID_TC0);  // enable clock for TC0, channel 0
    pmc_enable_periph_clk(ID_TC1);  // enable clock for TC0, channel 1
    
    pinMode(SCAN_LINE_PINS_PORT, scanLinePinsMask, OUTPUT);
    
    pinMode(PIOB, PIO_PB25B_TIOA0, PERIPHERAL_B); // give peripheral control to TC0.0 TIOA (Arduino pin 2)
    
    /* Connect TIOA1 (output of TC0 Channel 1) to XC0 (clock input) of TC0 Channel 0 */
    TC0->TC_BMR = TC_BMR_TC0XC0S_TIOA1;
    
    /* Configure output waveform clock */
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;  // disable clock
    TC0->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF;     // disable interrupts
    TC0->TC_CHANNEL[0].TC_SR;                   // clear status register
    TC0->TC_CHANNEL[0].TC_CMR = 
        TC_CMR_TCCLKS_TIMER_CLOCK2 |
        TC_CMR_BURST_XC0 |      // Gate the input clock with XC0 (connected to TIOA1)
        TC_CMR_WAVE |           // Waveform mode
        TC_CMR_WAVSEL_UP_RC |   // Counter running up and reset when equals to RC
        TC_CMR_EEVT_XC0 |       // Set external events from XC0 (this setup TIOB as output)
        TC_CMR_ACPA_NONE | TC_CMR_ACPC_TOGGLE |
        TC_CMR_BCPB_NONE | TC_CMR_BCPC_NONE ;
    TC0->TC_CHANNEL[0].TC_RC = 1;
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;
    
    /* Configure row switching clock */
    TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;  // disable clock
    TC0->TC_CHANNEL[1].TC_IDR = 0xFFFFFFFF;     // disable interrupts
    TC0->TC_CHANNEL[1].TC_SR;                   // clear status register
    TC0->TC_CHANNEL[1].TC_CMR = 
        TC_CMR_TCCLKS_TIMER_CLOCK2 |
        TC_CMR_CLKI |           // Increment on falling clock edge
        TC_CMR_WAVE |           // Waveform mode
        TC_CMR_WAVSEL_UP_RC |   // Counter running up and reset when equals to RC
        TC_CMR_CPCSTOP |        // Stop counter clock once it reaches RC
        TC_CMR_EEVT_XC0 |       // Set external events from XC0 (this setup TIOB as output)
        TC_CMR_ACPA_NONE |
        TC_CMR_ACPC_CLEAR |     // Clear TIOA on an RC Compare
        TC_CMR_ASWTRG_SET |     // Set TIOA on a software trigger.
        TC_CMR_BCPB_NONE | 
        TC_CMR_BCPC_NONE ;
    TC0->TC_CHANNEL[1].TC_RC = SEGMENT_TC_COUNT;    // Set RC. Switch scan lines every 512 clocks
    TC0->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;        // enable RC Compare interrupt
    TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN;

    TC0->TC_BCR = TC_BCR_SYNC;

    NVIC_SetPriority(TC1_IRQn, 0);
    NVIC_EnableIRQ(TC1_IRQn);

    /*
     * Configure the TLC5958 chips
     */
     
    TLC.FC1[0].BC = 0;
    TLC.replicateFC1();
    
    /* Set individual chip white-balance */
//    TLC.FC1[0].CCR = 110;
//    TLC.FC1[0].CCG = 70;
//    TLC.FC1[0].CCB = FC1_CCB_Max;
//    TLC.FC1[1].CCR = 110;
//    TLC.FC1[1].CCG = 100;
//    TLC.FC1[1].CCB = FC1_CCB_Max;
    
    TLC.FC1[0].CCB = FC1_CCB_Max;
    TLC.FC1[0].CCG = 275;
    TLC.FC1[0].CCR = 300;
    TLC.FC1[1].CCB = FC1_CCB_Max;
    TLC.FC1[1].CCG = 275;
    TLC.FC1[1].CCR = 280;
    
//    TLC.FC1[0].CCB = FC1_CCB_Max;
//    TLC.FC1[0].CCG = FC1_CCG_Max;
//    TLC.FC1[0].CCR = FC1_CCR_Max;
//    TLC.FC1[1].CCB = FC1_CCB_Max;
//    TLC.FC1[1].CCG = FC1_CCG_Max;
//    TLC.FC1[1].CCR = FC1_CCR_Max;
    
    
    TLC.FC1[0].SEL_GDLY =  FC1_SEL_GDLY_DISABLE;
    
    TLC.FC2[0].MAX_LINE = NUM_SCAN_LINES - 1;
    TLC.FC2[0].SEL_PWM = ES_PWM_DISPLAY_MODE;
    TLC.replicateFC2();
    
    TLC.WriteFC1();
    TLC.WriteFC2();
    
    /* Write all 0's to the memory bank */
    for (int j = 0; j < NUM_CHAINED_TLC5958; j++) {
        TLC.GS[j].OUTR = 0; 
        TLC.GS[j].OUTG = 0; 
        TLC.GS[j].OUTB = 0;
    }
    for (int r = 0; r < NUM_TLC_ROWS; r++) {
        for (int i = 0; i < COLS_PER_TLC5958; i++) {
            TLC.WriteGS();
        }
    }
    
    TLC.WriteVSYNC();
}


void updateDisplay()
{
    uint32_t now = millis();
    Pixel* pixel;
    for (register int r = 0; r < NUM_TLC_ROWS; r++) {
        for (register int i = 0; i < COLS_PER_TLC5958; i++) {
            for (register int j = 0; j < NUM_CHAINED_TLC5958; j++) {
                register int c = i + (j * COLS_PER_TLC5958);
                if (tlcToPixel_lut[r][c] > -1) {
                    pixel = &pixels[tlcToPixel_lut[r][c]];
                    pixel->update(now);
                    TLC.GS[j].OUTR = pixel->getCurrentR(); 
                    TLC.GS[j].OUTG = pixel->getCurrentG(); 
                    TLC.GS[j].OUTB = pixel->getCurrentB();
                }
            }
            TLC.WriteGS();
        }
    }
}



#ifdef __cplusplus
extern "C" {
#endif
     
// TC0, Channel 1 ISR
extern void TC1_IRQHandler(void)
{
    static volatile bool waitingIRQ = false;
    static volatile int scanLine = 0;
    static volatile int scanLinePinsVal = 0;
    
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
    uint32_t status = TC0->TC_CHANNEL[1].TC_SR; // Only triggered by RC Compare
            
    if (waitingIRQ)
    {
        waitingIRQ = false;
        TC0->TC_CHANNEL[1].TC_RC = SEGMENT_TC_COUNT;
        TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN;
        TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;
        TC0->TC_BCR = TC_BCR_SYNC;
    }
    else
    {
        if (doVsync && (scanLine == 0) && !TLC.isWriting()) // Frame is done and no current write operation
        {
            TLC.WriteVSYNC();
            doVsync = false;
        }
        
        digitalWriteSync(SCAN_LINE_PINS_PORT, scanLinePinsMask, ((1 << SCAN_LINE_PINS_OFFSET) << scanLine));
        
        scanLine++;
        if (scanLine >= NUM_SCAN_LINES) {
            scanLine = 0;
        }
        
        waitingIRQ = true;
        TC0->TC_CHANNEL[1].TC_RC = 1;
        TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
    }

}

#ifdef __cplusplus
}
#endif