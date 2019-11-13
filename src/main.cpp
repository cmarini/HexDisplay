
#include <sam3xa.h>
#include <stdio.h>


#include "TimerOne.h"
//#include "tlc5958.h"
#include "mymath.h"
#include "wiring_digital.h"
#include "wiring_analog.h"
#include "component/timetick.h"
#include "component/wiring.h"
#include "display.h"
#include "display_functions.h"
#include "atomicRTC.h"
//#include "audio_processing.h"

#define HARDFAULT_LED_ARUDINO_PIN 23

Tc* TC2_pnt = TC2;
Rtc* RTC_pnt = RTC;

/*
unsigned char ramp[101] = {
      0,   1,   1,   1,   2,   2,   3,   3,   4,   5, 
      5,   6,   6,   7,   8,   8,   9,  10,  10,  11, 
     12,  12,  13,  14,  15,  15,  16,  17,  17,  18, 
     19,  20,  21,  21,  22,  23,  24,  25,  26,  27, 
     27,  28,  29,  30,  31,  32,  33,  34,  35,  36, 
     37,  39,  40,  41,  42,  43,  44,  46,  47,  48, 
     50,  51,  52,  54,  55,  57,  58,  60,  62,  63, 
     65,  67,  69,  71,  73,  75,  77,  79,  82,  84, 
     87,  89,  92,  95,  99, 102, 105, 109, 113, 118, 
    123, 128, 134, 140, 148, 156, 167, 179, 195, 217, 
    255, 
};
*/
  


volatile bool whiteMode = false;
volatile bool buttonToggle = false;
bool ledState = true;

hsva_t hsvA = {0,S_VAL_LIMIT,V_VAL_LIMIT};
rgba_t rgb_white = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT};
rgba_t rgb_temp;

uint32_t now = 0;
uint32_t lastUpdate = 0;
uint32_t updateDelay = 30;
uint32_t lastAnalogRead = 0;

void buttonCallback(){
    buttonToggle = true;
}
void dipsw_cb_0(){
}
void dipsw_cb_1(){
}
void dipsw_cb_2(){
}
void dipsw_cb_3(){
}
void dipsw_cb_4(){
}
void dipsw_cb_5(){
}
void dipsw_cb_6(){
}
void dipsw_cb_7(){
}
unsigned char DIP_SW_pins[] = {
    46, 47, 48, 49, 
    50, 51, 52, 53,
};  
callback_t DIP_SW_callbacks[] = {
    dipsw_cb_0,
    dipsw_cb_1,
    dipsw_cb_2,
    dipsw_cb_3,
    dipsw_cb_4,
    dipsw_cb_5,
    dipsw_cb_6,
    dipsw_cb_7,
};

void setup()
{   
    pinMode_arduino(HARDFAULT_LED_ARUDINO_PIN, OUTPUT);
    digitalWrite_arduino(HARDFAULT_LED_ARUDINO_PIN, LOW);
    
    pmc_switch_sclk_to_32kxtal(0); // Use 32kHz Cyrstal instead of RC Osc.
    while(!pmc_osc_is_ready_32kxtal());
    SystemCoreClockUpdate();
    TimeTick_Configure(SystemCoreClock);
    pmc_set_writeprotect(0); // disable PMC write protection
    display_init();
    atomicRTC_init();
    randomInit();
    
    pmc_enable_periph_clk(ID_ADC);
    adc_init(SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
    
    for (int i = 0; i < 8; i++) {
        pinMode_arduino(DIP_SW_pins[i], INPUT_PULLUP);
        setDebounce_arduino(DIP_SW_pins[i], 20);
        attachInterrupt_arduino(DIP_SW_pins[i], CHANGE, DIP_SW_callbacks[i]);
    }
    
    pinMode_arduino(22, INPUT_PULLUP);
    setDebounce_arduino(22, 20);
    attachInterrupt_arduino(22, FALLING, buttonCallback);
    
    
}

bool didRead = false;
uint32_t startTime, endTime;
int32_t displayFuncNum = 0;
int32_t displayFuncNum_prev = -1;
int32_t displayFuncNum_buttonPrev = displayFuncNum;

void loop()
{
    
    now = millis();
    for( int i = 0; i < min(NUM_DISPLAY_FUNCS, 8); i++) {
        if (displayFunctions[i] != NULL) {
            if (digitalRead_arduino(DIP_SW_pins[i]) == LOW) {
                /* Display enabled */
                if (activeDisplayFuncs[i] == false) {
                    /* init */
                    activeDisplayFuncs[i] = true;
                    displayFunctions[i](STATE_INIT);
                }
                displayFunctions[i](STATE_RUN);
            } else {
                if (activeDisplayFuncs[i]) {
                    /* deinit */
                    activeDisplayFuncs[i] = false;
                    displayFunctions[i](STATE_DEINIT);
                }
            }
        }
    }
#if 0
    if (displayFuncNum != displayFuncNum_prev) {
        if (displayFuncNum < 0 || displayFuncNum >= NUM_DISPLAY_FUNCS) {
            displayFuncNum = 0;
        }
        /* deinit */
        if (!(displayFuncNum_prev < 0 || displayFuncNum_prev >= NUM_DISPLAY_FUNCS)) {
            if (displayFunctions[displayFuncNum_prev] != NULL) {
                displayFunctions[displayFuncNum_prev](STATE_DEINIT);
            }
        }
        displayFuncNum_prev = displayFuncNum;
        if (displayFunctions[displayFuncNum] != NULL) {
            displayFunctions[displayFuncNum](STATE_INIT);
        }
    }
    if (displayFunctions[displayFuncNum] != NULL) {
        displayFunctions[displayFuncNum](STATE_RUN);
    }
#endif
    
#if defined(_DISPLAY_TLC5958_h)
    if (!doVsync && (now - lastUpdate > 1)) {
        lastUpdate = now;
        startTime = millis();
        updateDisplay();
        endTime = millis();
        doVsync = true;
    }
#elif defined(_DISPLAY_SK6812_h)
    if (now - lastUpdate > updateDelay) {
        lastUpdate = now;
        startTime = millis();
        updateDisplay();
        endTime = millis();
    }
#else
#error NO DISPLAY UPDATE.
#endif
    
    if (buttonToggle) {
        buttonToggle = false;
        displayFuncNum++;
    }
    /*
    if (digitalRead_arduino(DIP_SW_pins[0]) == LOW) {
        TLC.WriteFC1();
        TLC.WriteFC2();
    }
    */
    
}




int main( void )
{
    __disable_irq();
	setup();
    __enable_irq();

	for (;;)
	{
		loop();
	}
}


#ifdef __cplusplus
extern "C" {
#endif
     
extern void HardFault_Handler(void)
{
    digitalWrite_arduino(HARDFAULT_LED_ARUDINO_PIN, HIGH);
    digitalWrite_arduino(HARDFAULT_LED_ARUDINO_PIN, HIGH);
}
extern void MemManage_Handler(void)
{
    digitalWrite_arduino(HARDFAULT_LED_ARUDINO_PIN, HIGH);
    digitalWrite_arduino(HARDFAULT_LED_ARUDINO_PIN, HIGH);
}

#ifdef __cplusplus
}
#endif


