#ifndef _DISPLAY_SK6812_h
#define _DISPLAY_SK6812_h

#include "display_sk6812.h"
#include "display.h"
#include "wiring_digital.h"
#include "component/pmc.h"
#include "component/wiring.h"
#include "mymath.h"
#include <sam3xa.h>



#define SK6812_BYTE_ZERO 0x80u
#define SK6812_BYTE_ONE 0xE0u
#define SK6812_SPI_CLOCK 3000000L

volatile unsigned long SPICLK;
void displayInterface_init()
{    
    /*
     * Set up SPI interface
     */
    
    SPICLK = SystemCoreClock / SK6812_SPI_CLOCK;
    
    pmc_enable_periph_clk(ID_SPI0);  // enable clock for SPI 0
    
    pinMode(PIOA, PIO_PA26A_SPI0_MOSI, PERIPHERAL_A); // give peripheral control to TC0.0 TIOA (Arduino pin 2)
    
    SPI0->SPI_WPMR = SPI_WPMR_WPKEY_PASSWD; // Disable write protection
    
    SPI0->SPI_CR = SPI_CR_SPIDIS; // Disable SPI0
    SPI0->SPI_CR = SPI_CR_SWRST;
    SPI0->SPI_CR = SPI_CR_SWRST;
    SPI0->SPI_CSR[0] = 
        SPI_CSR_BITS_8_BIT | 
        SPI_CSR_SCBR(SystemCoreClock / SK6812_SPI_CLOCK) | // Core clock divisor to get 4Mhz
        SPI_CSR_DLYBS(0) | 
        SPI_CSR_DLYBCT(0);
    SPI0->SPI_MR = 
        SPI_MR_MSTR | // Set to Master mode
        SPI_MR_PCS(0);
    // SPI0->SPI_WPMR = SPI_WPMR_WPEN | SPI_WPMR_WPKEY_PASSWD; // Enable write protection
    
    SPI0->SPI_CR = SPI_CR_SPIEN; // Enable SPI0
       
       
    pmc_enable_periph_clk(ID_DMAC);
    DMAC->DMAC_EN &= (~DMAC_EN_ENABLE);
    DMAC->DMAC_WPMR = DMAC_WPMR_WPKEY_PASSWD;
    DMAC->DMAC_GCFG = DMAC_GCFG_ARB_CFG_FIXED;
    DMAC->DMAC_EN = DMAC_EN_ENABLE;
}


const unsigned int CVAL_T_SIZE = sizeof(cval_t)*8;
#define NUM_COLORS 3
#define SPI_BUFF_OFFSET 1
#define SPI_BUFF_SIZE (PIXEL_COUNT * NUM_COLORS * CVAL_T_SIZE + SPI_BUFF_OFFSET)
uint8_t spi_buff[SPI_BUFF_SIZE];

#define pcb_to_idx(p,c,b) (((p)*(NUM_COLORS)*(CVAL_T_SIZE))+((c)*(CVAL_T_SIZE))+(b))

/** Use SAM3X DMAC if nonzero */
#define USE_SAM3X_DMAC 1
/** Use extra Bus Matrix arbitration fix if nonzero */
#define USE_SAM3X_BUS_MATRIX_FIX 0
/** Time in ms for DMA receive timeout */
#define SAM3X_DMA_TIMEOUT 100
/** chip select register number */
#define SPI_CHIP_SEL 3
/** DMAC receive channel */
#define SPI_DMAC_RX_CH  1
/** DMAC transmit channel */
#define SPI_DMAC_TX_CH  0
/** DMAC Channel HW Interface Number for SPI TX. */
#define SPI_TX_IDX  1
/** DMAC Channel HW Interface Number for SPI RX. */
#define SPI_RX_IDX  2


uint32_t calc_duration, dmac_duration;

void updateDisplay()
{
    
    // Disable DMAC Channel
    DMAC->DMAC_CHDR = DMAC_CHDR_DIS0 << SPI_DMAC_TX_CH;
    
    uint32_t startTime;
    
    startTime = millis();
    int p, c, b;
    int idx;
    rgb_t* raster = render();
    cval_t val[NUM_COLORS];
    for (p = 0; p < SPI_BUFF_OFFSET; p++) {
        spi_buff[p] = SK6812_BYTE_ZERO;
    }
    for (p = 0; p < PIXEL_COUNT; p++) {
        val[0] = raster[p].g;
        val[1] = raster[p].r;
        val[2] = raster[p].b;
        for (c = 0; c < NUM_COLORS; c++) {
            for (b = 0; b < CVAL_T_SIZE; b++) {
                idx = pcb_to_idx(p,c,b);
                spi_buff[idx + SPI_BUFF_OFFSET] = (val[c] >> (CVAL_T_SIZE-1-b))&0x1u ? SK6812_BYTE_ONE : SK6812_BYTE_ZERO;
            }
        }
    }
    //spi_buff[idx + SPI_BUFF_OFFSET + 1] = SK6812_BYTE_ZERO;
    calc_duration = millis() - startTime;
    startTime = millis();
    // Configure DMAC Channel
    DMAC->DMAC_CH_NUM[SPI_DMAC_TX_CH].DMAC_SADDR = (uint32_t)spi_buff;
    DMAC->DMAC_CH_NUM[SPI_DMAC_TX_CH].DMAC_DADDR = (uint32_t)&SPI0->SPI_TDR;
    DMAC->DMAC_CH_NUM[SPI_DMAC_TX_CH].DMAC_DSCR =  0;
    DMAC->DMAC_CH_NUM[SPI_DMAC_TX_CH].DMAC_CTRLA = 
        DMAC_CTRLA_BTSIZE(SPI_BUFF_SIZE) |
        DMAC_CTRLA_SRC_WIDTH_BYTE |
        DMAC_CTRLA_DST_WIDTH_BYTE;

    DMAC->DMAC_CH_NUM[SPI_DMAC_TX_CH].DMAC_CTRLB =
        DMAC_CTRLB_SRC_DSCR_FETCH_DISABLE |
        DMAC_CTRLB_DST_DSCR_FETCH_DISABLE |
        DMAC_CTRLB_FC_MEM2PER_DMA_FC |
        DMAC_CTRLB_SRC_INCR_INCREMENTING |
        DMAC_CTRLB_DST_INCR_FIXED;

    DMAC->DMAC_CH_NUM[SPI_DMAC_TX_CH].DMAC_CFG = 
        DMAC_CFG_DST_PER(SPI_TX_IDX) |
        DMAC_CFG_DST_H2SEL_HW |
        DMAC_CFG_SOD_ENABLE |
        DMAC_CFG_FIFOCFG_ALAP_CFG;


    // Enable DMAC Channel
    DMAC->DMAC_CHER = DMAC_CHER_ENA0 << SPI_DMAC_TX_CH;
    
    while(DMAC->DMAC_CHSR & (DMAC_CHSR_ENA0 << SPI_DMAC_TX_CH));
    
    dmac_duration = millis() - startTime;
    __NOP();
    /* Bit-bang
    for (p = 0; p < PIXEL_COUNT; p++) {
        for (c = 0; c < 3; c++) {
            for (b = 0; b < CVAL_T_SIZE; b++) {
                while (!(SPI0->SPI_SR & SPI_SR_TDRE));
                SPI0->SPI_TDR = spi_buff[p][c][b];
            }
        }
    }
    */
}


/*

#ifdef __cplusplus
extern "C" {
#endif
     
// TC0, Channel 1 ISR
extern void TC1_IRQHandler(void)
{

}

#ifdef __cplusplus
}
#endif

*/

#endif /* _DISPLAY_SK6812_h */

