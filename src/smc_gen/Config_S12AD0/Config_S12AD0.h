/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_S12AD0.h
* Component Version: 2.5.0
* Device(s)        : R5F513T3AxFJ
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
