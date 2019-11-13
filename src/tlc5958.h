#ifndef _TLC5958_h
#define _TLC5958_h

#include <stdint.h>
#include "mymath.h"


// TLC5958 commands defined as number of clock edges

#define CMD_FCWRTEN 15
#define CMD_VSYNC   3
#define CMD_WRTGS   1
#define CMD_WRTFC   5
#define     WRTFC_Pos   44
#define     WRTFC_Msk   0xfu
#define     WRTFC_FC1   0x9u
#define     WRTFC_FC2   0x6u

/* Function Control Register 1 */

#define     FC1_LODVTH_Pos          0
#define     FC1_LODVTH_Msk          0x3u
#define         FC1_LODVTH_VLOD0        0
#define         FC1_LODVTH_VLOD1        1
#define         FC1_LODVTH_VLOD2        2
#define         FC1_LODVTH_VLOD3        3

#define     FC1_SEL_TD0_Pos         2
#define     FC1_SEL_TD0_Msk         0x3u
#define         FC1_SEL_TD0_0           0
#define         FC1_SEL_TD0_1           1
#define         FC1_SEL_TD0_2           2
#define         FC1_SEL_TD0_3           3

#define     FC1_SEL_GDLY_Pos        4
#define     FC1_SEL_GDLY_Msk        0x1u
#define         FC1_SEL_GDLY_DISABLE    0x0u
#define         FC1_SEL_GDLY_ENABLE     0x1u

#define     FC1_LGSE1B_Pos          5
#define     FC1_LGSE1B_Msk          0x3u
#define         FC1_LGSE1B_NONE         0
#define         FC1_LGSE1B_WEAK         1
#define         FC1_LGSE1B_MEDIUM       2
#define         FC1_LGSE1B_STRONG       3
            // BIT 7 RESERVED
#define     FC1_LGSE1G_Pos          8
#define     FC1_LGSE1G_Msk          0x3u
#define         FC1_LGSE1G_NONE         0
#define         FC1_LGSE1G_WEAK         1
#define         FC1_LGSE1G_MEDIUM       2
#define         FC1_LGSE1G_STRONG       3
            // BIT 10 RESERVED
#define     FC1_LGSE1R_Pos          11
#define     FC1_LGSE1R_Msk          0x3u
#define         FC1_LGSE1R_NONE         0
#define         FC1_LGSE1R_WEAK         1
#define         FC1_LGSE1R_MEDIUM       2
#define         FC1_LGSE1R_STRONG       3
            // BIT 13 RESERVED
#define     FC1_CCB_Pos             14
#define     FC1_CCB_Msk             0x1FFu
#define     FC1_CCB_Max             511
        
#define     FC1_CCG_Pos             23
#define     FC1_CCG_Msk             0x1FFu
#define     FC1_CCG_Max             511
        
#define     FC1_CCR_Pos             32
#define     FC1_CCR_Msk             0x1FFu
#define     FC1_CCR_Max             511
        
#define     FC1_BC_Pos              41
#define     FC1_BC_Msk              0x7u
#define     FC1_BC_Max              7

/* Function Control Register 2 */

#define     FC2_MAX_LINE_Pos        0
#define     FC2_MAX_LINE_Msk        0x1fu
#define     FC2_MAX_LINE_Max        32

#define     FC2_PSAVE_ENA_Pos       5
#define     FC2_PSAVE_ENA_Msk       0x1u
#define         FC2_PSAVE_ENA_DISABLE       0x0u
#define         FC2_PSAVE_ENA_ENABLE        0x1u

#define     FC2_SEL_GCLK_EDGE_Pos   6
#define     FC2_SEL_GCLK_EDGE_Msk   0x1u
#define         FC2_SEL_GCLK_EDGE_RISING    0x0u
#define         FC2_SEL_GCLK_EDGE_BOTH      0x1u

#define     FC2_SEL_PCHG_Pos        7
#define     FC2_SEL_PCHG_Msk        0x1u
#define         FC2_SEL_PCHG_DISABLE        0x0u
#define         FC2_SEL_PCHG_ENABLE         0x1u

            // BITS 8-10 RESERVED
#define     FC2_EMI_REDUCE_B_Pos    11
#define     FC2_EMI_REDUCE_B_Msk    0x1u
#define         FC2_EMI_REDUCE_B_DISABLE    0x0u
#define         FC2_EMI_REDUCE_B_ENABLE     0x1u

#define     FC2_EMI_REDUCE_G_Pos    12
#define     FC2_EMI_REDUCE_G_Msk    0x1u
#define         FC2_EMI_REDUCE_G_DISABLE    0x0u
#define         FC2_EMI_REDUCE_G_ENABLE     0x1u

#define     FC2_EMI_REDUCE_R_Pos    13
#define     FC2_EMI_REDUCE_R_Msk    0x1u
#define         FC2_EMI_REDUCE_R_DISABLE    0x0u
#define         FC2_EMI_REDUCE_R_ENABLE     0x1u

#define     FC2_SEL_PWM_Pos         14
#define     FC2_SEL_PWM_Msk         0x1u
#define         FC2_SEL_PWM_8P8             0x0u
#define         FC2_SEL_PWM_9P7             0x1u


#define     FC2_LGSE2_Pos           15
#define     FC2_LGSE2_Msk           0x3u
#define         FC2_LGSE2_NONE              0
#define         FC2_LGSE2_WEAK              1
#define         FC2_LGSE2_MEDIUM            2
#define         FC2_LGSE2_STRONG            3

/* Grayscale Data Register */

#define     GS_OUTR_Pos 0
#define     GS_OUTR_Msk 0xffffu
#define     GS_OUTR_Max 0xffffu

#define     GS_OUTG_Pos 16
#define     GS_OUTG_Msk 0xffffu
#define     GS_OUTG_Max 0xffffu

#define     GS_OUTB_Pos 32
#define     GS_OUTB_Msk 0xffffu
#define     GS_OUTB_Max 0xffffu

#define     GS_OUT_Max  min(GS_OUTR_Max, min(GS_OUTG_Max, GS_OUTB_Max))

typedef struct {
    uint32_t OUTR   :16;
    uint32_t OUTG   :16;
    uint32_t OUTB   :16;
} GS_t;

typedef struct {
    uint32_t LODVTH     :2;
    uint32_t SEL_TD0    :2;
    uint32_t SEL_GDLY   :1;
    uint32_t LGSE1B     :2;
    uint32_t _Reserved1 :1;
    uint32_t LGSE1G     :2;
    uint32_t _Reserved2 :1;
    uint32_t LGSE1R     :2;
    uint32_t _Reserved3 :1;
    uint32_t CCB        :9;
    uint32_t CCG        :9;
    uint32_t CCR        :9;
    uint32_t BC         :3;
    uint32_t WRTFC      :4;
} FC1_t;
        
typedef struct {
    uint32_t MAX_LINE       :5;
    uint32_t PSAVE_ENA      :1;
    uint32_t SEL_GCLK_EDGE  :1;
    uint32_t SEL_PCHG       :1;
    uint32_t _Reserved1     :3;
    uint32_t EMI_REDUCE_B   :1;
    uint32_t EMI_REDUCE_G   :1;
    uint32_t EMI_REDUCE_R   :1;
    uint32_t SEL_PWM        :1;
    uint32_t LGSE2          :2;
    uint32_t _Reserved2     :15; // up to word boundary
    uint32_t _Reserved3     :12;
    uint32_t WRTFC          :4;
} FC2_t;

class TLC5958
{
    public:
        // Properties  
        GS_t *GS;
        FC1_t *FC1;
        FC2_t *FC2;
    

        // Methods
        TLC5958(uint32_t chainLength);
        void replicateFC1();
        void replicateFC2();
        void WriteGS();
        void WriteVSYNC();
        void WriteFCWRTEN();
        void WriteFC1();
        void WriteFC2();
        bool isWriting();
        
    private:
        // Properties
        bool writeLock;
        unsigned int chainLength;
        
        // Methods
        void WriteBufferCMD(void* buff, uint8_t cmd);
        static void ShiftOutMSB(void* buff, uint8_t pos, uint8_t len);
};

//extern TLC5958 TLC;

#endif /* _TLC5958_h */
