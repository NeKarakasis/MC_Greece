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
* File Name   : r_motor_driver.h
* Description : Definitions for motor driver module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_DRIVER_H
#define R_MOTOR_DRIVER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_module_cfg.h"

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/* Copy structure definition from motor component, to avoid build error in non motor component project*/
#ifndef MTR_ADC_TB
#define MTR_ADC_TB
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
#endif

typedef struct st_motor_driver
{
    /* The function pointer for AdcGetConvVal function in motor component */
    void (*ADCDataGet)(r_mtr_adc_tb * mtr_ad_data);

    /* The function pointer for UpdDuty function in motor component */
    void (*BLDCDutySet)(uint16_t f4_duty_u, uint16_t f4_duty_v, uint16_t f4_duty_w);

    /* The function pointer for UpdZeroDuty function in motor component */
    void (*BLDCZeroDutySet)(void);

    /* The function pointer for UpdCompareDuty function in motor component */
    void (*BLDCCompareDutySet)(void);

    /* The function pointer for StopTimerCtrl function in motor component */
    void (*PWMOutputStop)(void);

    /* The function pointer for StartTimerCtrl function in motor component */
    void (*PWMOutputStart)(void);

    float f4_ad_crnt_per_digit;                         /* The current[A] per ADC digit */
    float f4_ad_vdc_per_digit;                          /* The voltage[V] per ADC digit */
    float f4_pwm_period_cnt;                            /* The number of counts per PWM period  */
    float f4_pwm_dead_time_cnt;                         /* The number of counts of PWM deadtime */
} st_motor_driver_t;

typedef struct
{
    /* The function pointer for AdcGetConvVal function in motor component */
    void (*ADCDataGet)(r_mtr_adc_tb * mtr_ad_data);

    /* The function pointer for UpdDuty function in motor component */
    void (*BLDCDutySet)(uint16_t f4_duty_u, uint16_t f4_duty_v, uint16_t f4_duty_w);

    /* The function pointer for UpdZeroDuty function in motor component */
    void (*BLDCZeroDutySet)(void);

    /* The function pointer for UpdCompareDuty function in motor component */
    void (*BLDCCompareDutySet)(void);

    /* The function pointer for StopTimerCtrl function in motor component */
    void (*PWMOutputStop)(void);

    /* The function pointer for StartTimerCtrl function in motor component */
    void (*PWMOutputStart)(void);

    float f4_shunt_ohm;                                 /* The resistance [ohm] of shunt resister */
    float f4_volt_gain;                                 /* The gain value of (actual voltage)/(voltage on ADC input) */
    float f4_crnt_amp_gain;                             /* The gain value of (actual current)/(voltage on ADC input) */
    float f4_pwm_period_cnt;                            /* The number of counts per PWM period  */
    float f4_pwm_dead_time_cnt;                         /* The number of counts of PWM deadtime */
} st_motor_driver_cfg_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_motor_driver_t g_st_driver;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_MOTOR_DRIVER_Open(void);
void R_MOTOR_DRIVER_Close(void);
void R_MOTOR_DRIVER_ParameterUpdate(st_motor_driver_t * p_st_driver, const st_motor_driver_cfg_t * p_st_driver_cfg);
void R_MOTOR_DRIVER_BldcAnalogGet(st_motor_driver_t * p_st_driver,float f4_duty_u,float f4_duty_v,float f4_duty_w, float * p_f4_iu_ad,float * p_f4_iv_ad, float * p_f4_iw_ad, float * p_f4_vdc_ad);
void R_MOTOR_DRIVER_BldcDutySet(st_motor_driver_t * p_st_driver, float f4_duty_u, float f4_duty_v, float f4_duty_w);
void R_MOTOR_DRIVER_BldcZeroDutySet(st_motor_driver_t * p_st_driver);
void R_MOTOR_DRIVER_BldcCompareDutySet(st_motor_driver_t * p_st_driver);
void R_MOTOR_DRIVER_PWMControlStop(st_motor_driver_t * p_st_driver);
void R_MOTOR_DRIVER_PWMControlStart(st_motor_driver_t * p_st_driver);

#endif /* R_MOTOR_DRIVER_H */
