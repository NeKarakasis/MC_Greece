/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_S12AD0.h
* Component Version: 2.5.0
* Device(s)        : R5F513T3AxFL
* Description      : This file implements device driver for Config_S12AD0.
***********************************************************************************************************************/

#ifndef CFG_Config_S12AD0_H
#define CFG_Config_S12AD0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_s12ad.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _0021_AD_PGA_STABLE_WAIT_COUNT     (0x0021U) /* PGA stabilization 5us wait time */
#define _0012_AD0_DSH_SAMPLING_STATE       (0x0012U) /* Dedicated sample-and-hold circuit sampling time setting */
#define _06_AD0_SAMPLING_STATE_0           (0x06U)   /* AN000/Self-diagnosis sampling time setting */
#define _06_AD0_SAMPLING_STATE_1           (0x06U)   /* AN001 sampling time setting */
#define _06_AD0_SAMPLING_STATE_2           (0x06U)   /* AN002 sampling time setting */
#define _06_AD0_SAMPLING_STATE_3           (0x06U)   /* AN003 sampling time setting */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_S12AD0_Create(void);
void R_Config_S12AD0_Create_UserInit(void);
void R_Config_S12AD0_Start(void);
void R_Config_S12AD0_Stop(void);
void R_Config_S12AD0_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);
/* Start user code for function. Do not edit comment generated here */
#ifndef MTR_ADC_TB
#define MTR_ADC_TB
#endif
typedef struct
{
    uint16_t   u2_iu_ad;
    uint16_t   u2_iv_ad;
    uint16_t   u2_iw_ad;
    uint16_t   u2_vdc_ad;
    uint16_t   u2_vphase_u_ad;
    uint16_t   u2_vphase_v_ad;
    uint16_t   u2_vphase_w_ad;
} r_mtr_adc_tb;

void R_Config_MOTOR_AdcGetConvVal( r_mtr_adc_tb *mtr_ad_data );
/* End user code. Do not edit comment generated here */
#endif
