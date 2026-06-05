/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_motor_driver_fsp.h
* Description : Definitions for FSP driver module
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_DRIVER_FSP_H
#define R_MOTOR_DRIVER_FSP_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_driver.h"
#include "r_pfc_driver.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
/* GPT PWM MODULATION LIMIT */
#define GPT_PEAK_CNT            (1875U)
#define GPT_MAX_CNT             (1874U)
#define GPT_MIN_CNT             (1U)

#define PFC_PORT_RELAY          (BSP_IO_PORT_14_PIN_01)        /* Output port of PE01 */
#define PFC_PORT_THERMAL        (BSP_IO_PORT_13_PIN_07)        /* Input port of PD07 */

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
#ifndef MTR_ADC_TB
#define MTR_ADC_TB
typedef struct
{
    uint16_t   u2_iu_ad;              /* U phase current (register value) */
    uint16_t   u2_iv_ad;              /* V phase current (register value) */
    uint16_t   u2_iw_ad;              /* W phase current (register value) */
    uint16_t   u2_vdc_ad;             /* Vdc voltage (register value) */
    uint16_t   u2_vphase_u_ad;        /* U phase voltage (register value) */
    uint16_t   u2_vphase_v_ad;        /* V phase voltage (register value) */
    uint16_t   u2_vphase_w_ad;        /* W phase voltage (register value) */
} r_mtr_adc_tb;
#endif

#ifndef PFC_ADC_TB
#define PFC_ADC_TB
typedef struct
{
    uint16_t   u2_vac_ad;         /* Vac voltage (register value) */
    uint16_t   u2_vdc_ad;         /* Vdc voltage (register value) */
    uint16_t   u2_ichp_ad;        /* Ichp current (register value) */
} r_pfc_adc_tb;
#endif

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_Config_MOTOR_StartTimerCtrl(void);
void R_Config_MOTOR_StopTimerCtrl(void);
void R_Config_MOTOR_UpdDuty(uint16_t duty_u, uint16_t duty_v, uint16_t duty_w);
void R_Config_MOTOR_UpdZeroDuty(void);
void R_Config_MOTOR_UpdCompareDuty(void);
void R_Config_MOTOR_AdcGetConvVal(r_mtr_adc_tb * mtr_ad_data);
void R_Config_PFC_StartTimerCtrl(void);
void R_Config_PFC_StopTimerCtrl(void);
void R_Config_PFC_AdcGetConvVal(r_pfc_adc_tb * pfc_ad_data);
void R_Config_PFC_UpdDuty(uint16_t u2_duty_cnt);
void R_Config_PFC_RelayOn(void);
void R_Config_PFC_RelayOff(void);
uint8_t R_Config_PFC_OverheatCheck(void);

#endif /* R_MOTOR_DRIVER_FSP_H */
