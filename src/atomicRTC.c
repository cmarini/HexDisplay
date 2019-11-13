
#include "atomicRTC.h"
#include <sam3xa.h>
#include "component/pmc.h"
#include "wiring_digital.h"
#include "mymath.h"
#include <time.h>

volatile bool doAtomicSync = false;
int consecutiveSyncs = 0;
bool doDoubleSync = true;

/* externals */
volatile bool rtcSecTick = false;
void (*rtcSecTickCallback)(void) = NULL;

const int atomicEnablePin_arduino = 4;


#define ARTC_TIMER_FREQ     32768

void startAtomicSync()
{
    digitalWrite_arduino(atomicEnablePin_arduino, LOW);
    NVIC_SetPriority(TC6_IRQn, 15);
    NVIC_EnableIRQ(TC6_IRQn);
    consecutiveSyncs = 0;
    doAtomicSync = true;
}

void stopAtomicSync()
{
    NVIC_DisableIRQ(TC6_IRQn);
    doAtomicSync = false;
    digitalWrite_arduino(atomicEnablePin_arduino, HIGH);
}

bool isAtomicSyncing()
{
    return doAtomicSync;
}

void atomicRTC_init()
{
    /*
     * TC2 Channel 0
     * TIOA6, PC25, Arduino pin 5
     */
    
    pmc_enable_periph_clk(ID_TC6);  // enable clock for TC2, channel 0
    pinMode(PIOC, PIO_PC25B_TIOA6, PERIPHERAL_B); // give peripheral control to TC2.0 TIOA
    
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;  // disable clock
    TC2->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF;     // disable interrupts
    TC2->TC_CHANNEL[0].TC_SR;                   // clear status register
    TC2->TC_CHANNEL[0].TC_CMR = 
        TC_CMR_TCCLKS_TIMER_CLOCK5 |    // Slow clock
        TC_CMR_ETRGEDG_RISING |         // External trigger on rising edges
        TC_CMR_ABETRG |                 // Use TIOA as external trigger
        TC_CMR_LDRA_FALLING |           // Load RA on falling edge of TIOA
        TC_CMR_CPCTRG |                 // RC Compare resets counter (for pulse timeout)
        0;
    TC2->TC_CHANNEL[0].TC_IER = TC_IER_LDRAS | TC_IER_CPCS; // Interrupt on RA load and RC compare
    TC2->TC_CHANNEL[0].TC_RC = ARTC_TIMER_FREQ; // 1 second pulse timeout
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;
    
    /*
     * Configure Real-Time Clock
     */
     
    pmc_enable_periph_clk(ID_RTC);  // enable clock for the Real-time Clock
    RTC->RTC_MR = 0; // 24-hour mode (RTC_MR_HRMOD for 12-hour mode)
    RTC->RTC_CR |= RTC_CR_TIMEVSEL_MIDNIGHT; // Generate an interrupt every midnight
    RTC->RTC_SCCR = RTC_SCCR_TIMCLR; // Clear status bit
    RTC->RTC_IER = RTC_IER_TIMEN | RTC_IER_SECEN; // Enable midnight and second interrupt
    
    NVIC_SetPriority(RTC_IRQn, 15);
    NVIC_EnableIRQ(RTC_IRQn);
    pinMode_arduino(atomicEnablePin_arduino, OUTPUT);
    startAtomicSync();
}


#define toBCD(val)      (((((val)/10)%10) << 4) | ((val)%10))
#define BCDtoInt(val)   ((((val)>>0)&0xf)*1 + (((val)>>4)&0xf)*10)

volatile uint64_t pulseReadBits = 0;

typedef union {
    struct {
        uint8_t sec         :7;
        uint8_t Reserved1   :1;
        uint8_t min         :7;
        uint8_t Reserved2   :1;
        uint8_t hour        :6;
        uint8_t ampm        :1;
    } b;
    uint32_t word;
} RTC_REG_TIMR_t;

typedef union {
    struct {
        uint8_t cent        :7;
        uint8_t Reserved1   :1;
        uint8_t year        :8;
        uint8_t month       :5;
        uint8_t day         :3;
        uint8_t date        :6;
    } b;
    uint32_t word;
} RTC_REG_CALR_t;

RTC_REG_TIMR_t rtcTimeUpdate;
RTC_REG_CALR_t rtcCalUpdate;

struct tm* RTCtoLocaltime(struct tm* time)
{
    RTC_REG_CALR_t cal;
    RTC_REG_TIMR_t tim;
    tim.word = RTC->RTC_TIMR;
    if (!(RTC->RTC_TIMR == tim.word))
    {
        tim.word = RTC->RTC_TIMR;
    }
    cal.word = RTC->RTC_CALR;
    
    time->tm_sec = BCDtoInt(tim.b.sec);
    time->tm_min = BCDtoInt(tim.b.min);
    time->tm_hour = BCDtoInt(tim.b.hour);
    time->tm_mday = BCDtoInt(cal.b.date);
    time->tm_mon = BCDtoInt(cal.b.month) - 1;
    time->tm_year = (BCDtoInt(cal.b.cent)*100 + BCDtoInt(cal.b.year)) - 1900;
    
    mktime(time);
    return time;
}

time_t RTCtoTime()
{
    struct tm time;
    return mktime(RTCtoLocaltime(&time));
}

void updateRTC(time_t t)
{
    struct tm time;
    localtime_r(&t, &time);
    RTC->RTC_IDR = RTC_IDR_ACKDIS; // Disable the RTC update interrupt
    
    rtcTimeUpdate.b.sec = toBCD(time.tm_sec);
    rtcTimeUpdate.b.min = toBCD(time.tm_min);
    rtcTimeUpdate.b.hour = toBCD(time.tm_hour);
    
    rtcCalUpdate.b.cent = toBCD((time.tm_year + 1900)/100);
    rtcCalUpdate.b.year = toBCD(time.tm_year - 100);
    rtcCalUpdate.b.month = toBCD(time.tm_mon + 1);
    rtcCalUpdate.b.day = toBCD(time.tm_wday + 1);
    rtcCalUpdate.b.date = toBCD(time.tm_mday);
    
    RTC->RTC_IER = RTC_IER_ACKEN; // Enable the RTC update interrupt
    RTC->RTC_CR |= RTC_CR_UPDTIM | RTC_CR_UPDCAL;
}

#include "wwvb_code.h"

#define WWVB_MARKER_LEN     26400 /* Real data calibration */ // (ARTC_TIMER_FREQ*8/10) // Spec: 0.8s = 26214 = 32768 * 0.8
#define WWVB_W_CODE_LEN     17000 /* Real data calibration */ // (ARTC_TIMER_FREQ*5/10) // Spec: 0.5s = 16384 = 32768 * 0.5
#define WWVB_U_CODE_LEN     7700  /* Real data calibration */ // (ARTC_TIMER_FREQ*2/10) // Spec: 0.5s = 6553  = 32768 * 0.2

#define WWVB_MARKER_RANGE_MAX (WWVB_MARKER_LEN + ((ARTC_TIMER_FREQ-WWVB_MARKER_LEN)/4))
#define WWVB_W_CODE_RANGE_MAX (WWVB_W_CODE_LEN + ((WWVB_MARKER_LEN-WWVB_W_CODE_LEN)/4))
#define WWVB_U_CODE_RANGE_MAX (WWVB_U_CODE_LEN + ((WWVB_W_CODE_LEN-WWVB_U_CODE_LEN)/4))

#define WWVB_MARKER_RANGE_MIN (WWVB_MARKER_LEN - ((WWVB_MARKER_LEN-WWVB_W_CODE_LEN)/4))
#define WWVB_W_CODE_RANGE_MIN (WWVB_W_CODE_LEN - ((WWVB_W_CODE_LEN-WWVB_U_CODE_LEN)/4))
#define WWVB_U_CODE_RANGE_MIN (WWVB_U_CODE_LEN - ((WWVB_U_CODE_LEN-0              )/4))

const int TIME_ZONE_OFFSET = -5;

time_t decodeWWVB(uint64_t bits)
{
    /* Decode frame */
    uint8_t daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = WWVB_DAYS(bits); /* 1-indexed */
    if (days > 366) return 0;
    int month = 0;
    /* Turn 'days' into the right month and day */
    for (month = 0; month < 12; month++) {
        int dpm = daysPerMonth[month] + (month==1 ? WWVB_LEAP_YEAR(bits): 0);
        if (days > dpm) {
            days -= dpm;
        } else {
            break;
        }
    }
    
    if(WWVB_MIN(bits) > 59) return 0;
    if(WWVB_HOUR(bits) > 23) return 0;
    if(WWVB_YEAR(bits) > 99) return 0;
    
    struct tm time;
    time.tm_sec = 0;
    time.tm_min = WWVB_MIN(bits) + 1; /* We just read the previous minute */
    time.tm_hour = WWVB_HOUR(bits);
    time.tm_mday = days; /* day of the month, 1 to 31 */
    time.tm_mon = month; /* months since January, 0 to 11 */
    time.tm_year = 100 + WWVB_YEAR(bits); /* struct tm: years since 1900 */
    time.tm_hour += TIME_ZONE_OFFSET;
    
    /*  Adjust for DST
            00  DST has not been in effect for over a day. Apply standard time correction 
                with respect to UTC. Next transition is into DST.
            01  DST ends today. Continue to apply the DST correction and return to 
                standard time only once the appropriate instance has been reached.
            10  DST starts today. Do not apply the 1 hour DST correction until the 
                appropriate time has been reached. 
            11  DST has been in effect for more than a day. Apply one hour less of time 
                correction with respect to UTC. Next transition is out of DST.
    */
    if (WWVB_DST(bits) == 0x3 ||
        (WWVB_DST(bits) == 0x1 && time.tm_hour < 2) ||
        (WWVB_DST(bits) == 0x2 && time.tm_hour >= 2)
    ) {
        time.tm_hour += 1;
    }
    
    return mktime(&time); /* format everything into appropriate ranges */
}

void decodeAndUpdate()
{
    static time_t time_prev;
    time_t time = decodeWWVB(pulseReadBits);
    
    if (doDoubleSync && consecutiveSyncs == 0)
    {   /* This is just the first sync. Go do another for comparison */
        time_prev = time;
        consecutiveSyncs++;
        return;
    }
    
    if (doDoubleSync || 
        (!doDoubleSync && abs(RTCtoTime() - time) > 60*60*3)
    ) 
    {   /* We need consecutive readings */
        if (consecutiveSyncs > 0)
        {   /* There's a previous sync to compare */
            if (time_prev + 60 == time)
            {   /* Everything looks good. Go ahead and set RTC */
                doDoubleSync = false;
            }
            else
            {   /* Syncs don't jive! Keep trying. */
                time_prev = time;
//                consecutiveSyncs = max(0, consecutiveSyncs - 1);
                return;
            }
        }
        else 
        {   /* Non-initial sync is way off from RTC. Confirm with another sync. */
            doDoubleSync = true;
            time_prev = time;
            consecutiveSyncs++;
            return;
        }
    }
    
    stopAtomicSync();
    consecutiveSyncs = 0;
    updateRTC(time);
    return;
}


volatile int markerCount = 0;
volatile int frameBitCount = 0;

volatile int ssi = 0;
int historyCount = 0;
#define MAX_HISTORY 10
#if MAX_HISTORY > 0xFFFFFFFFFFFFFFFFu
    #error MAX_HISTORY too big for a uint64_t
#elsif MAX_HISTORY > 0xFFFFFFFFu
    uint64_t readHistory = 0;
#else
    uint32_t readHistory = 0;
#endif
    
int getAtomicSSI()
{
    return ssi;
}
int getFrameBitCount()
{
    return frameBitCount;
}

uint32_t reads[256];
int readsIdx = 0;
    
#ifdef __cplusplus
extern "C" {
#endif
/* TC2, Channel 0 ISR */
extern void TC6_IRQHandler(void)
{
    static int consecutiveMarkers = 0;
    static bool goodRead;
    
    uint32_t status = TC2->TC_CHANNEL[0].TC_SR;
    if (!doAtomicSync) return;
    
    goodRead = false;
    
    if (status & TC_SR_CPCS)
    {   /* RC compare. That means pulse timeout */
        goodRead = false;
    }
    else if (status & TC_SR_LDRAS)
    {   /* RA */
        uint32_t ra = TC2->TC_CHANNEL[0].TC_RA;
        TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
        reads[readsIdx++] = ra;
        if (readsIdx >= 256) {
            readsIdx = 0;
        }
        
        if (ra > 0 && ra < ARTC_TIMER_FREQ)
        {   /* Treat as a valid pulse duration */
            pulseReadBits <<= 1;
            if (ra >= WWVB_MARKER_RANGE_MIN && ra <= WWVB_MARKER_RANGE_MAX)
            {   /* Marker */
                markerCount++;
                frameBitCount++;
                goodRead = true;
                if (++consecutiveMarkers == 2)
                {   /* This is the first pulse of new frame */
                    markerCount = 1;
                    frameBitCount = 1;
                }
                else if (markerCount == 7 && frameBitCount == 60)
                {   /* This is the last pulse of a frame. Now deal with it! */
                    decodeAndUpdate();
                    markerCount = 0;
                    frameBitCount = 0;
                }
            }
            else if (ra >= WWVB_W_CODE_RANGE_MIN && ra <= WWVB_W_CODE_RANGE_MAX)
            {   /* Binary 1 */
                consecutiveMarkers = 0;
                frameBitCount++;
                pulseReadBits |= 1;
                goodRead = true;
            }
            else if (ra >= WWVB_U_CODE_RANGE_MIN && ra <= WWVB_U_CODE_RANGE_MAX)
            {   /* Binary 0 */
                consecutiveMarkers = 0;
                frameBitCount++;
                goodRead = true;
            }
            else
            {   /* Reading too far off. Start again! */
            }
        }
        else
        {   /* Probably didn't have a good signal. Start Again! */
        }
    }
    else
    {   /* Unknown status flag */
        return;
    }

    historyCount = min(MAX_HISTORY, historyCount + 1);
    readHistory <<= 1LL;
    readHistory |= goodRead ? 1 : 0;
    if (goodRead)
    {
    }
    else
    {
        markerCount = 0;
        frameBitCount = 0;
    }
    int goodReads = 0;
    for (int i = 0; i < historyCount; i++)
    {
        goodReads += (readHistory>>i) & 0x1;
    }
    ssi = goodReads * 100 / historyCount;
}


/* Real-time Clock ISR */
extern void RTC_IRQHandler(void)
{
    uint32_t status = RTC->RTC_SR;
    if (status & RTC_SR_ALARM)
    {
        RTC->RTC_SCCR = RTC_SCCR_ALRCLR;
    }
    if (status & RTC_SR_TIMEV)
    {
        RTC->RTC_SCCR = RTC_SCCR_TIMCLR;
        startAtomicSync();
    }
    if (status & RTC_SR_ACKUPD)
    {   /* Time and Cal registers can be updated */
        RTC->RTC_SCCR = RTC_SCCR_ACKCLR;
        uint32_t mode = RTC->RTC_MR;
        RTC->RTC_MR = 0; // Set 24-hour mode
        
        RTC->RTC_TIMR = *(uint32_t*)&rtcTimeUpdate;
        RTC->RTC_CALR = *(uint32_t*)&rtcCalUpdate;
        
        RTC->RTC_MR = mode;
        RTC->RTC_CR &= ~(RTC_CR_UPDTIM | RTC_CR_UPDCAL);
    }
    if (status & RTC_SR_SEC)
    {
        RTC->RTC_SCCR = RTC_SCCR_SECCLR;
        rtcSecTick = true;
        if (rtcSecTickCallback != NULL)
        {
            rtcSecTickCallback();
        }
    }
    RTC->RTC_SCCR = status; // Clear any remaining status flags
}

#ifdef __cplusplus
}
#endif

