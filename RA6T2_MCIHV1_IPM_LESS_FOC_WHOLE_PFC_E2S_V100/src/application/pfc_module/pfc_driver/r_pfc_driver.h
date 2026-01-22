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
* Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : r_pfc_driver.h
* Description : Definitions for PFC driver module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_PFC_DRIVER_H
#define R_PFC_DRIVER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Global structure
***********************************************************************************************************************/
#ifndef PFC_ADC_TB
#define PFC_ADC_TB
typedef struct
{
    uint16_t   u2_vac_ad;
    uint16_t   u2_vdc_ad;
    uint16_t   u2_ichp_ad;
} r_pfc_adc_tb;
#endif

typedef struct st_pfc_driver
{
    /* The function pointer for AdcGetConvVal function in PFC component */
    void (*ADCDataGet)(r_pfc_adc_tb * pfc_ad_data);

    /* The function pointer for UpdDuty function in PFC component */
    void (*DutySet)(uint16_t u2_duty_u);

    /* The function pointer for StopTimerCtrl function in PFC component */
    void (*PWMOutputStop)(void);

    /* The function pointer for StartTimerCtrl function in PFC component */
    void (*PWMOutputStart)(void);

    /* The function pointer for RelayOn function in PFC component */
    void (*RelayOn)(void);

    /* The function pointer for RelayOff function in PFC component */
    void (*RelayOff)(void);

    /* The function pointer for OverheatCheck function in PFC component */
    uint8_t (*OverheatCheck)(void);

    float f4_ad_vac_per_digit;                          /* The AC voltage[V] per ADC digit */
    float f4_ad_vdc_per_digit;                          /* The bus voltage[V] per ADC digit */
    float f4_ad_crnt_per_digit;                         /* The current[A] per ADC digit */
    float f4_pwm_period_cnt;                            /* The number of counts per PWM period */
    int16_t s2_input_volt_offset;                       /* The offset value of actual input voltage adjustment */
    int16_t s2_bus_volt_offset;                         /* The offset value of actual bus voltage adjustment */
    int16_t s2_pfc_crnt_offset;                         /* The offset value of actual PFC current adjustment */
    uint16_t u2_pwm_valid_max_cnt;                      /* The number of PWM valid maximum counts */
    uint16_t u2_pwm_valid_min_cnt;                      /* The number of PWM valid minimum counts */
} st_pfc_driver_t;

typedef struct
{
    /* The function pointer for AdcGetConvVal function in PFC component */
    void (*ADCDataGet)(r_pfc_adc_tb * pfc_ad_data);

    /* The function pointer for UpdDuty function in PFC component */
    void (*DutySet)(uint16_t u2_duty);

    /* The function pointer for StopTimerCtrl function in PFC component */
    void (*PWMOutputStop)(void);

    /* The function pointer for StartTimerCtrl function in PFC component */
    void (*PWMOutputStart)(void);

    /* The function pointer for RelayOn function in PFC component */
    void (*RelayOn)(void);

    /* The function pointer for RelayOff function in PFC component */
    void (*RelayOff)(void);

    /* The function pointer for OverheatCheck function in PFC component */
    uint8_t (*OverheatCheck)(void);

    float f4_shunt_ohm;                                 /* The resistance [ohm] of shunt resister */
    float f4_ac_volt_gain;                              /* The gain value of (actual AC voltage)/(voltage on ADC input) */
    float f4_bus_volt_gain;                             /* The gain value of (actual bus voltage)/(voltage on ADC input) */
    float f4_crnt_amp_gain;                             /* The gain value of (actual current)/(voltage on ADC input) */
    float f4_pwm_period_cnt;                            /* The number of counts per PWM period */
    int16_t s2_input_volt_offset;                       /* The offset value of actual input voltage adjustment */
    int16_t s2_bus_volt_offset;                         /* The offset value of actual bus voltage adjustment */
    int16_t s2_pfc_crnt_offset;                         /* The offset value of actual PFC current adjustment */
    uint16_t u2_pwm_valid_max_cnt;                      /* The number of PWM valid maximum counts */
    uint16_t u2_pwm_valid_min_cnt;                      /* The number of PWM valid minimum counts */
} st_pfc_driver_cfg_t;

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
extern st_pfc_driver_t g_st_pfc_driver;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void R_PFC_DRIVER_Open(void);
void R_PFC_DRIVER_Close(void);
void R_PFC_DRIVER_ParameterUpdate(st_pfc_driver_t * p_st_pfc_driver,
                                  const st_pfc_driver_cfg_t * p_st_pfc_driver_cfg);
void R_PFC_DRIVER_AnalogGet(st_pfc_driver_t * p_st_pfc_driver,
                            float * p_f4_vac_ad,
                            float * p_f4_vdc_ad,
                            float * p_f4_ichp_ad);
void R_PFC_DRIVER_DutySet(st_pfc_driver_t * p_st_pfc_driver, float f4_duty);
void R_PFC_DRIVER_PWMControlStop(st_pfc_driver_t * p_st_pfc_driver);
void R_PFC_DRIVER_PWMControlStart(st_pfc_driver_t * p_st_pfc_driver);
void R_PFC_DRIVER_RelayOn(st_pfc_driver_t * p_st_pfc_driver);
void R_PFC_DRIVER_RelayOff(st_pfc_driver_t * p_st_pfc_driver);
uint8_t R_PFC_DRIVER_OverheatCheck(st_pfc_driver_t * p_st_pfc_driver);

#endif /* R_PFC_DRIVER_H */
