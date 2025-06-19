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
*         : 07.12.2021 1.00
*         : 12.05.2022 1.10
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_DRIVER_H
#define R_MOTOR_DRIVER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_smc_entry.h"
#include "r_motor_module_cfg.h"

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
/* Copy structure definition from motor component, to avoid build error in non motor component project*/
#ifndef MTR_ADC_TB
#define MTR_ADC_TB
typedef struct
{
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
    uint16_t   u2_vdc_ad;
    uint16_t   u2_Iac_raw0;
    uint16_t   u2_Iac_raw1;
#else
    uint16_t   u2_iu_ad;
    uint16_t   u2_iv_ad;
    uint16_t   u2_iw_ad;
    uint16_t   u2_vdc_ad;
    uint16_t   u2_vphase_u_ad;
    uint16_t   u2_vphase_v_ad;
    uint16_t   u2_vphase_w_ad;
#endif
} r_mtr_adc_tb;
#endif

typedef struct st_motor_driver
{
    /* The function pointer for AdcGetConvVal function in motor component */
    void (*ADCDataGet)(r_mtr_adc_tb * mtr_ad_data);

    /* The function pointer for UpdDuty function in motor component */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#else
    void (*BLDCDutySet)(uint16_t f4_duty_u, uint16_t f4_duty_v, uint16_t f4_duty_w);
#endif

    /* The function pointer for StopTimerCtrl function in motor component */
    void (*PWMOutputStop)(void);

    /* The function pointer for StartTimerCtrl function in motor component */
    void (*PWMOutputStart)(void);

    float f4_ad_crnt_per_digit;                         /* The current[A] per ADC digit */
    float f4_ad_vdc_per_digit;                          /* The coltage[V] per ADC digit */
    float f4_pwm_period_cnt;                            /* The number of counts per PWM period  */
    float f4_pwm_dead_time_cnt;                         /* The number of counts of PWM deadtime */
} st_motor_driver_t;

typedef struct
{
    /* The function pointer for AdcGetConvVal function in motor component */
    void (*ADCDataGet)(r_mtr_adc_tb * mtr_ad_data);

    /* The function pointer for UpdDuty function in motor component */
#if defined(MOTOR_SHUNT_TYPE_1_SHUNT)
#else
    void (*BLDCDutySet)(uint16_t f4_duty_u, uint16_t f4_duty_v, uint16_t f4_duty_w);
#endif

    /* The function pointer for StopTimerCtrl function in motor component */
    void (*PWMOutputStop)(void);

    /* The function pointer for StartTimerCtrl function in motor component */
    void (*PWMOutputStart)(void);

    float f4_shunt_ohm;                                 /* The registance [ohm] of shunt resister */
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
/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_Open
* Description   : Open a motor driver instance
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_Open(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_Close
* Description   : Close driver module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_Close(void);

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_ParameterUpdate
* Description   : Update the motor driver parameter
* Arguments     : p_st_driver     - The pointer to the motor driver structure
*                 p_st_driver_cfg - The pointer to the motor driver configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_ParameterUpdate(st_motor_driver_t * p_st_driver,
                                    const st_motor_driver_cfg_t * p_st_driver_cfg);

#if defined(MOTOR_SHUNT_TYPE_2_SHUNT)
/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcAnalogGet
* Description   : Get the current and bus voltage
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 p_f4_iu_ad  - The U phase current A/D conversion value (pointer)
*                 p_f4_iw_ad  - The W phase current A/D conversion value (pointer)
*                 p_f4_vdc_ad - The DC bus voltage A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcAnalogGet(st_motor_driver_t * p_st_driver,
                                  float f4_duty_u,
                                  float f4_duty_v,
                                  float f4_duty_w,
                                  float * p_f4_iu,
                                  float * p_f4_iv,
                                  float * p_f4_iw,
                                  float * p_f4_vdc);
#elif defined(MOTOR_SHUNT_TYPE_1_SHUNT)
/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_1ShuntBldcAnalogGet
* Description   : Get the current and bus voltage for 1shunt
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 p_f4_Iac_ad - Phase current A/D conversion value
*                 p_f4_vdc_ad - Bus voltage A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_1ShuntBldcAnalogGet(st_motor_driver_t * p_st_driver,
                                        float * p_f4_Iac_ad,
                                        float * p_f4_vdc_ad);
#endif

#if defined(MOTOR_SHUNT_TYPE_2_SHUNT)
/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcDutySet
* Description   : PWM duty setting
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 f4_duty_u   - The duty cycle of Phase-U (0.0 - 1.0);
*                 f4_duty_v   - The duty cycle of Phase-V (0.0 - 1.0);
*                 f4_duty_w   - The duty cycle of Phase-W (0.0 - 1.0);
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcDutySet(st_motor_driver_t * p_st_driver, float f4_duty_u, float f4_duty_v, float f4_duty_w);
#endif

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_PWMControlStop
* Description   : Stop PWM outputs
* Arguments     : p_st_driver - The pointer to the motor driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_PWMControlStop(st_motor_driver_t * p_st_driver);

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_PWMControlStart
* Description   : Start PWM outputs
* Arguments     : p_st_driver - The pointer to the motor driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_PWMControlStart(st_motor_driver_t * p_st_driver);

#endif /* R_MOTOR_DRIVER_H */
