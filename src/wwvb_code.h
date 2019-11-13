#ifndef WWVB_CODE_h
#define WWVB_CODE_h

    /*
     * WWVB Time Code Format
     */
    
    #define WWVB_DST_pos            1
    #define WWVB_DST_msk            0x3u
    #define WWVB_DST(val)           (((val) >> WWVB_DST_pos) & WWVB_DST_msk)
    
    #define WWVB_LEAP_SEC_pos       3
    #define WWVB_LEAP_SEC_msk       0x1u
    #define WWVB_LEAP_SEC(val)      (((val) >> WWVB_LEAP_SEC_pos) & WWVB_LEAP_SEC_msk)
    
    #define WWVB_LEAP_YEAR_pos      4
    #define WWVB_LEAP_YEAR_msk      0x1u
    #define WWVB_LEAP_YEAR(val)     (((val) >> WWVB_LEAP_YEAR_pos) & WWVB_LEAP_YEAR_msk)
    
    #define WWVB_YEAR_BCD1_pos      6
    #define WWVB_YEAR_BCD1_msk      0xFu
    #define WWVB_YEAR_BCD1(val)     (((val) >> WWVB_YEAR_BCD1_pos) & WWVB_YEAR_BCD1_msk)
    #define WWVB_YEAR_BCD10_pos     11
    #define WWVB_YEAR_BCD10_msk     0xFu
    #define WWVB_YEAR_BCD10(val)    (((val) >> WWVB_YEAR_BCD10_pos) & WWVB_YEAR_BCD10_msk)
    #define WWVB_YEAR(val)          (WWVB_YEAR_BCD1(val) + 10*WWVB_YEAR_BCD10(val))
    
    #define WWVB_CORR_pos           16
    #define WWVB_CORR_msk           0xFu
    #define WWVB_CORR(val)          (((val) >> WWVB_CORR_pos) & WWVB_CORR_msk)
    
    #define WWVB_SIGN_pos           21
    #define WWVB_SIGN_msk           0x7u
    #define WWVB_SIGN(val)          (((val) >> WWVB_SIGN_pos) & WWVB_SIGN_msk)
    
    #define WWVB_DAYS_BCD1_pos      26
    #define WWVB_DAYS_BCD1_msk      0xFu
    #define WWVB_DAYS_BCD1(val)     (((val) >> WWVB_DAYS_BCD1_pos) & WWVB_DAYS_BCD1_msk)
    #define WWVB_DAYS_BCD10_pos     31
    #define WWVB_DAYS_BCD10_msk     0xFu
    #define WWVB_DAYS_BCD10(val)    (((val) >> WWVB_DAYS_BCD10_pos) & WWVB_DAYS_BCD10_msk)
    #define WWVB_DAYS_BCD100_pos    36
    #define WWVB_DAYS_BCD100_msk    0x3u
    #define WWVB_DAYS_BCD100(val)   (((val) >> WWVB_DAYS_BCD100_pos) & WWVB_DAYS_BCD100_msk)
    #define WWVB_DAYS(val)          (WWVB_DAYS_BCD1(val) + 10*WWVB_DAYS_BCD10(val) + 100*WWVB_DAYS_BCD100(val))
    
    #define WWVB_HOUR_BCD1_pos      41
    #define WWVB_HOUR_BCD1_msk      0xFu
    #define WWVB_HOUR_BCD1(val)     (((val) >> WWVB_HOUR_BCD1_pos) & WWVB_HOUR_BCD1_msk)
    #define WWVB_HOUR_BCD10_pos     46
    #define WWVB_HOUR_BCD10_msk     0x3u
    #define WWVB_HOUR_BCD10(val)    (((val) >> WWVB_HOUR_BCD10_pos) & WWVB_HOUR_BCD10_msk)
    #define WWVB_HOUR(val)          (WWVB_HOUR_BCD1(val) + 10*WWVB_HOUR_BCD10(val))
    
    #define WWVB_MIN_BCD1_pos       51
    #define WWVB_MIN_BCD1_msk       0xFu
    #define WWVB_MIN_BCD1(val)      (((val) >> WWVB_MIN_BCD1_pos) & WWVB_MIN_BCD1_msk)
    #define WWVB_MIN_BCD10_pos      56
    #define WWVB_MIN_BCD10_msk      0x7u
    #define WWVB_MIN_BCD10(val)     (((val) >> WWVB_MIN_BCD10_pos) & WWVB_MIN_BCD10_msk)
    #define WWVB_MIN(val)           (WWVB_MIN_BCD1(val) + 10*WWVB_MIN_BCD10(val))
   


#endif /* WWVB_CODE_h */
