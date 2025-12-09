/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MOTOR.h
* Component Version: 1.1.0
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_MOTOR.
***********************************************************************************************************************/

#ifndef CFG_Config_MOTOR_H
#define CFG_Config_MOTOR_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_MTR_PRIORITY_LEVEL0      (0x00U) /* Level 0 (interrupt disabled) */
#define _01_MTR_PRIORITY_LEVEL1      (0x01U) /* Level 1 */
#define _02_MTR_PRIORITY_LEVEL2      (0x02U) /* Level 2 */
#define _03_MTR_PRIORITY_LEVEL3      (0x03U) /* Level 3 */
#define _04_MTR_PRIORITY_LEVEL4      (0x04U) /* Level 4 */
#define _05_MTR_PRIORITY_LEVEL5      (0x05U) /* Level 5 */
#define _06_MTR_PRIORITY_LEVEL6      (0x06U) /* Level 6 */
#define _07_MTR_PRIORITY_LEVEL7      (0x07U) /* Level 7 */
#define _08_MTR_PRIORITY_LEVEL8      (0x08U) /* Level 8 */
#define _09_MTR_PRIORITY_LEVEL9      (0x09U) /* Level 9 */
#define _0A_MTR_PRIORITY_LEVEL10     (0x0AU) /* Level 10 */
#define _0B_MTR_PRIORITY_LEVEL11     (0x0BU) /* Level 11 */
#define _0C_MTR_PRIORITY_LEVEL12     (0x0CU) /* Level 12 */
#define _0D_MTR_PRIORITY_LEVEL13     (0x0DU) /* Level 13 */
#define _0E_MTR_PRIORITY_LEVEL14     (0x0EU) /* Level 14 */
#define _0F_MTR_PRIORITY_LEVEL15     (0x0FU) /* Level 15 (highest) */

//CNT_CLK,CNT_CLK2
#define _00_MTR_MTU_PCLK_1           (0x00U)
#define _01_MTR_MTU_PCLK_2           (0x01U)
#define _01_MTR_MTU_PCLK_4           (0x01U)
#define _02_MTR_MTU_PCLK_8           (0x02U)
#define _02_MTR_MTU_PCLK_16          (0x02U)
#define _03_MTR_MTU_PCLK_32          (0x03U)
#define _03_MTR_MTU_PCLK_64          (0x03U)
#define _04_MTR_MTU_PCLK_256         (0x04U)
#define _05_MTR_MTU_PCLK_1024        (0x05U)
//CLK_EDG
#define _00_MTR_MTU_CKEG_RISE        (0x00U)

//TOCR_PSYE_VALUE
#define  _08_MTR_MTU_TOCL_DISABLE            (0x08U)
#define  _04_MTR_MTU_TOCS_TOCR2              (0x04U)

#define  _00_MTR_MTU_PSYE_DISABLE            (0x00U)
#define  _01_MTR_MTU_TGIEA_ENABLE            (0x01U)
#define  _10_MTR_MTU_TCIEV_ENABLE            (0x10U)
#define  _02_MTR_MTU_TGIEB_ENABLE            (0x02U)
//AD conversion trigger timing
#define  _0002_MTR_MTU_TADCORA_VALUE         (0x0002U)
//AD trigger count direction
#define  _0040_MTR_MTU_DTAE_ENABLE           (0x0040U)
//INTSKP-AD Synch
#define  _0000_MTR_MTU_ADSYNCH_DISABLE       (0x0000U)
#define  _0008_MTR_MTU_ADSYNCH_ITAAE_ENABLE  (0x0008U)
#define  _0004_MTR_MTU_ADSYNCH_ITAVE_ENABLE  (0x0004U)
//Interrupt skipping
#define  _00_MTR_MTU_INT_SKIP_DISABLE        (0x00U)
#define  _09_MTR_MTU_INT_SKIP_COUNT_1        (0x09U)
#define  _0A_MTR_MTU_INT_SKIP_COUNT_2        (0x0AU)
#define  _0B_MTR_MTU_INT_SKIP_COUNT_3        (0x0BU)
#define  _0C_MTR_MTU_INT_SKIP_COUNT_4        (0x0CU)
#define  _0D_MTR_MTU_INT_SKIP_COUNT_5        (0x0DU)
#define  _0E_MTR_MTU_INT_SKIP_COUNT_6        (0x0EU)
#define  _0F_MTR_MTU_INT_SKIP_COUNT_7        (0x0FU)

#define  _0D_MTR_MTU_CMT1                    (0x0DU)
#define  _0E_MTR_MTU_CMT2                    (0x0EU)
#define  _10_MTR_MTU_BFA_BUFFER              (0x10U)
#define  _20_MTR_MTU_BFB_BUFFER              (0x20U)

#define  _10_MTR_MTU_SCH3_START              (0x10U)
#define  _08_MTR_MTU_SCH4_START              (0x08U)
#define  _02_MTR_MTU_SCH6_START              (0x02U)
#define  _01_MTR_MTU_SCH7_START              (0x01U)

//S12AD
#define  _0000_MTR_AD_SINGLE_SCAN_MODE          (0x0000U)
#define  _1000_MTR_AD_SCAN_END_INTERRUPT_ENABLE (0x1000U)

#define  _0001_MTR_AD_AN000_USED                (0x0001U)
#define  _0002_MTR_AD_AN001_USED                (0x0002U)
#define  _0004_MTR_AD_AN002_USED                (0x0004U)
#define  _0008_MTR_AD_AN003_USED                (0x0008U)
#define  _0010_MTR_AD_AN004_USED                (0x0010U)
#define  _0020_MTR_AD_AN005_USED                (0x0020U)
#define  _0040_MTR_AD_AN006_USED                (0x0040U)
#define  _0080_MTR_AD_AN007_USED                (0x0080U)
#define  _0001_MTR_AD_AN016_USED                (0x0001U)
#define  _0002_MTR_AD_AN017_USED                (0x0002U)

#define  _0001_MTR_AD_AN100_USED                (0x0001U)
#define  _0002_MTR_AD_AN101_USED                (0x0002U)
#define  _0004_MTR_AD_AN102_USED                (0x0004U)
#define  _0008_MTR_AD_AN103_USED                (0x0008U)


#define  _0010_MTR_AD_AN104_USED                (0x0010U)
#define  _0020_MTR_AD_AN105_USED                (0x0020U)
#define  _0040_MTR_AD_AN106_USED                (0x0040U)
#define  _0080_MTR_AD_AN107_USED                (0x0080U)
#define  _0100_MTR_AD_AN108_USED                (0x0100U)
#define  _0200_MTR_AD_AN109_USED                (0x0200U)
#define  _0400_MTR_AD_AN110_USED                (0x0400U)
#define  _0800_MTR_AD_AN111_USED                (0x0800U)
#define  _1000_MTR_AD_AN112_USED                (0x1000U)
#define  _2000_MTR_AD_AN113_USED                (0x2000U)
#define  _4000_MTR_AD_AN114_USED                (0x4000U)
#define  _8000_MTR_AD_AN115_USED                (0x8000U)
#define  _0001_MTR_AD_AN116_USED                (0x0001U)
#define  _0002_MTR_AD_AN117_USED                (0x0002U)
#define  _0004_MTR_AD_AN118_USED                (0x0004U)
#define  _0008_MTR_AD_AN119_USED                (0x0008U)
#define  _0010_MTR_AD_AN120_USED                (0x0010U)

#define  _0001_MTR_AD_AN200_USED                (0x0001U)
#define  _0002_MTR_AD_AN201_USED                (0x0002U)
#define  _0004_MTR_AD_AN202_USED                (0x0004U)
#define  _0008_MTR_AD_AN203_USED                (0x0008U)
#define  _0010_MTR_AD_AN204_USED                (0x0010U)
#define  _0020_MTR_AD_AN205_USED                (0x0020U)
#define  _0040_MTR_AD_AN206_USED                (0x0040U)
#define  _0080_MTR_AD_AN207_USED                (0x0080U)
#define  _0100_MTR_AD_AN208_USED                (0x0100U)
#define  _0200_MTR_AD_AN209_USED                (0x0200U)
#define  _0400_MTR_AD_AN210_USED                (0x0400U)
#define  _0800_MTR_AD_AN211_USED                (0x0800U)
#define  _0001_MTR_AD_AN216_USED                (0x0001U)
#define  _0002_MTR_AD_AN217_USED                (0x0002U)

#define  _01_MTR_AD_DSH_AN000_SELECT            (0x01U)
#define  _02_MTR_AD_DSH_AN001_SELECT            (0x02U)
#define  _04_MTR_AD_DSH_AN002_SELECT            (0x04U)
#define  _01_MTR_AD_DSH_AN100_SELECT            (0x01U)
#define  _02_MTR_AD_DSH_AN101_SELECT            (0x02U)
#define  _04_MTR_AD_DSH_AN102_SELECT            (0x04U)
 
#define  _0900_MTR_AD_TRSA_TRG4AN                (0x0900U)
#define  _0D00_MTR_AD_TRSA_TRG7AN                (0x0D00U)

/*
    Timer Output Master Enable Registers (TOERA)
*/
/* Master Enable MTIOC3B (OE3B) */
#define _C1_MTR_MTU_OE3B_ENABLE             (0xC1U) /* MTU output is enabled */
/* Master Enable MTIOC4A (OE4A) */
#define _C2_MTR_MTU_OE4A_ENABLE             (0xC2U) /* MTU output is enabled */
/* Master Enable MTIOC4B (OE4B) */
#define _C4_MTR_MTU_OE4B_ENABLE             (0xC4U) /* MTU output is enabled */
/* Master Enable MTIOC3D (OE3D) */
#define _C8_MTR_MTU_OE3D_ENABLE             (0xC8U) /* MTU output is enabled */
/* Master Enable MTIOC4C (OE4C) */
#define _D0_MTR_MTU_OE4C_ENABLE             (0xD0U) /* MTU output is enabled */
/* Master Enable MTIOC4D (OE4D) */
#define _E0_MTR_MTU_OE4D_ENABLE             (0xE0U) /* MTU output is enabled */

/*
    Timer Output Master Enable Registers (TOERB)
*/
/* Master Enable MTIOC6B (OE6B) */
#define _C1_MTR_MTU_OE6B_ENABLE             (0xC1U) /* MTU output is enabled */
/* Master Enable MTIOC7A (OE7A) */
#define _C2_MTR_MTU_OE7A_ENABLE             (0xC2U) /* MTU output is enabled */
/* Master Enable MTIOC7B (OE7B) */
#define _C4_MTR_MTU_OE7B_ENABLE             (0xC4U) /* MTU output is enabled */
/* Master Enable MTIOC6D (OE6D) */
#define _C8_MTR_MTU_OE6D_ENABLE             (0xC8U) /* MTU output is enabled */
/* Master Enable MTIOC7C (OE7C) */
#define _D0_MTR_MTU_OE7C_ENABLE             (0xD0U) /* MTU output is enabled */
/* Master Enable MTIOC7D (OE7D) */
#define _E0_MTR_MTU_OE7D_ENABLE             (0xE0U) /* MTU output is enabled */

/*
    Timer Output Control Registers 2 (TOCR2A, TOCR2B)
*/
/* Output Level Select 1P (OLS1P) */
#define _00_MTR_MTU_OLS1P_HL                (0x00U) /* Initial output:H, Active level:L */
#define _01_MTR_MTU_OLS1P_LH                (0x01U) /* Initial output:L, Active level:H */
/* Output Level Select 1N (OLS1N) */
#define _00_MTR_MTU_OLS1N_HL                (0x00U) /* Initial output:H, Active level:L */
#define _02_MTR_MTU_OLS1N_LH                (0x02U) /* Initial output:L, Active level:H */
/* Output Level Select 2P (OLS2P) */
#define _00_MTR_MTU_OLS2P_HL                (0x00U) /* Initial output:H, Active level:L */
#define _04_MTR_MTU_OLS2P_LH                (0x04U) /* Initial output:L, Active level:H */
/* Output Level Select 2N (OLS2N) */
#define _00_MTR_MTU_OLS2N_HL                (0x00U) /* Initial output:H, Active level:L */
#define _08_MTR_MTU_OLS2N_LH                (0x08U) /* Initial output:L, Active level:H */
/* Output Level Select 3P (OLS3P) */
#define _00_MTR_MTU_OLS3P_HL                (0x00U) /* Initial output:H, Active level:L */
#define _10_MTR_MTU_OLS3P_LH                (0x10U) /* Initial output:L, Active level:H */
/* Output Level Select 3N (OLS3N) */
#define _00_MTR_MTU_OLS3N_HL                (0x00U) /* Initial output:H, Active level:L */
#define _20_MTR_MTU_OLS3N_LH                (0x20U) /* Initial output:L, Active level:H */
/* TOLBR Buffer Transfer Timing Select (BF[1:0]) */
#define _00_MTR_MTU_TOLBR_DIS               (0x00U) /* Does not transfer data from the buffer register (TOLBR) to TOCR2 */
#define _40_MTR_MTU_TOLBR_C                 (0x40U) /* Transfers at the crest of the MTU4,7.TCNT count */
#define _80_MTR_MTU_TOLBR_T                 (0x80U) /* Transfers at the trough of the MTU4,7.TCNT count */
#define _C0_MTR_MTU_TOLBR_CT                (0xC0U) /* Transfers at crest and trough of the MTU4,7.TCNT count */

#define _0040_3TCNT_VALUE            (0x0040U) /* MTU3.TCNT value */
#define _0040_TDDRA_VALUE            (0x0040U) /* MTU.TDDRA value */

#define  _0A69_TCDRA_VALUE           (0x0A69U)
#define  _0A69_TCDRB_VALUE           (0x0A69U)
#define _0A69_TCDRA_VALUE            (0x0A69U) /* MTU.TCDRA value */
#define _0554_3TGRB_VALUE            (0x0554U) /* MTU3.TGRB value */
#define _0554_4TGRA_VALUE            (0x0554U) /* MTU4.TGRA value */
#define _0554_4TGRB_VALUE            (0x0554U) /* MTU4.TGRB value */
#define _0AA9_SUM_VALUE              (0x0AA9U) /* Timer General Register (TGR) value */

/*
    Macro definitions for Motor Middleware
*/
#define CG_CONFIG_MOTOR_PWM_TIMER_FREQ            (32.0f)          /* PWM timer frequency [MHz] */
#define CG_CONFIG_MOTOR_CARRIER_FREQ              (6.003f)        /* Carrier wave actual frequency [kHz] */
#define CG_CONFIG_MOTOR_DEADTIME                  (2.000f)        /* Deadtime actual value [us] */
#define CG_CONFIG_MOTOR_INTR_DECIMATION           (0.0)            /* Interrupt skipping number */
#define CG_MOTOR_CFG_MAX_AD_DATA                      (4095.0f)        /* Max A/D data */
#define CG_MOTOR_MCU_CFG_AD_FREQ                      (32.0f)          /* A/D frequency [MHz] */


#ifndef MTR_ADC_TS
#define MTR_ADC_TS
typedef struct
{
    uint16_t   u2_ia_ad;
    uint16_t   u2_ib_ad;
    uint16_t   u2_vdc_ad;
    uint16_t   u2_vphase_a_ad;
    uint16_t   u2_vphase_b_ad;
} r_mtr_adc_ts;
#endif


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_MOTOR_Create(void);
void R_Config_MOTOR_Create_UserInit(void);
void R_Config_MOTOR_StartTimerCount(void);
void R_Config_MOTOR_StopTimerCount(void);
void R_Config_MOTOR_StartTimerCtrl(void);
void R_Config_MOTOR_StopTimerCtrl(void);
void R_Config_MOTOR_UpdDuty(uint16_t duty_u, uint16_t duty_v, uint16_t duty_w);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
