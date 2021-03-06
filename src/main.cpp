
#include <sam3xa.h>
#include <stdio.h>

// #define SWO(...) do{if(DEBUG) printf(__VA_ARGS__);}while(0)
#define SWO(...) __NOP();

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

volatile bool whiteMode = false;
volatile bool buttonToggle = false;
bool ledState = true;

hsva_t hsvA = {0,S_VAL_LIMIT,V_VAL_LIMIT};
rgba_t rgb_white = {R_VAL_LIMIT, G_VAL_LIMIT, B_VAL_LIMIT};
rgba_t rgb_temp;

uint32_t now = 0;
uint32_t lastUpdate = 0;
uint32_t updateDelay = 0;
uint32_t lastAnalogRead = 0;

volatile uint32_t framerate = 0;
uint32_t framerate_count = 0;
uint32_t framerate_start = 0;
uint32_t framerate_end = 0;

void buttonCallback(){
    buttonToggle = true;
}
void dipsw_cb_0(){
    SWO("DIPSW 0");
}
void dipsw_cb_1(){
    SWO("DIPSW 1");
}
void dipsw_cb_2(){
    SWO("DIPSW 2");
}
void dipsw_cb_3(){
    SWO("DIPSW 3");
}
void dipsw_cb_4(){
    SWO("DIPSW 4");
}
void dipsw_cb_5(){
    SWO("DIPSW 5");
}
void dipsw_cb_6(){
    SWO("DIPSW 6");
}
void dipsw_cb_7(){
    SWO("DIPSW 7");
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
uint32_t displayFuncTime = 0;
uint32_t displayUpdateTime = 0;

int32_t displayFuncNum = 0;
int32_t displayFuncNum_prev = -1;
int32_t displayFuncNum_buttonPrev = displayFuncNum;

void loop()
{
    
    now = millis();
    startTime = millis();
    for( int i = 0; i < min(NUM_DISPLAY_FUNCS, 8); i++) {
        if (displayFunctions[i] != NULL) {
            if (digitalRead_arduino(DIP_SW_pins[i]) == LOW) {
                /* Display enabled */
                if (activeDisplayFuncs[i] == false) {
                    /* init */
                    SWO("Init displayFunction[%d]...", i);
                    activeDisplayFuncs[i] = true;
                    displayFunctions[i](STATE_INIT);
                    SWO("Done\n");
                }
                displayFunctions[i](STATE_RUN);
            } else {
                if (activeDisplayFuncs[i]) {
                    /* deinit */
                    SWO("Deinit displayFunction[%d]...", i);
                    activeDisplayFuncs[i] = false;
                    displayFunctions[i](STATE_DEINIT);
                    SWO("Done\n");
                }
            }
        }
    }
    displayFuncTime = millis() - startTime;
    
    
#if defined(_DISPLAY_TLC5958_h)
    if (!doVsync && (now - lastUpdate > 1)) {
        lastUpdate = now;
        startTime = millis();
        updateDisplay();
        endTime = millis();
        doVsync = true;
    }
#elif defined(_DISPLAY_SK6812_h)
    if (millis() >= framerate_start + 1000) {
        uint32_t timediff = millis() - framerate_start;
        framerate = (framerate_count*1000)/timediff;
        framerate_count = 0;
        framerate_start = millis();
    }
    if (now - lastUpdate > updateDelay) {
        lastUpdate = now;
        startTime = millis();
        updateDisplay();
        displayUpdateTime = millis() - startTime;
        framerate_count++;
    }
#else
#error NO DISPLAY UPDATE.
#endif
    
    if (buttonToggle) {
        buttonToggle = false;
        displayFuncNum++;
    }
    
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


