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
* File Name   : r_motor_driver.c
* Description : The processes of motor driver module
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 29.02.2024 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_driver.h"
#include "r_motor_module_cfg.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#if defined(INVERTER_CFG_ADC_REF_VOLTAGE)
#define MOTOR_DRIVER_PRV_ADC_REF_VOLTAGE          (INVERTER_CFG_ADC_REF_VOLTAGE) /* AD reference voltage */
#else
#define MOTOR_DRIVER_PRV_ADC_REF_VOLTAGE          (5.0f)                         /* Default AD reference voltage */
#endif

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
st_motor_driver_t g_st_driver;

/***********************************************************************************************************************
* Global functions definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_Open
* Description   : Open a motor driver instance
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_Open(void)
{
    st_motor_driver_t * p_st_driver;

    p_st_driver = &g_st_driver;

    p_st_driver->f4_ad_crnt_per_digit = 0.0f;
    p_st_driver->f4_ad_vdc_per_digit  = 0.0f;
    p_st_driver->f4_pwm_period_cnt    = 0.0f;
    p_st_driver->f4_pwm_dead_time_cnt = 0.0f;
} /* End of function R_MOTOR_DRIVER_Open */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_Close
* Description   : Close driver module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_Close(void)
{
    /* No structure member need to be reset in Close function */
} /* End of function R_MOTOR_DRIVER_Close */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_ParameterUpdate
* Description   : Update the motor driver parameter
* Arguments     : p_st_driver     - The pointer to the motor driver structure
*                 p_st_driver_cfg - The pointer to the motor driver configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_ParameterUpdate(st_motor_driver_t * p_st_driver, const st_motor_driver_cfg_t * p_st_driver_cfg)
{
    float f4_volt_per_digit;

    f4_volt_per_digit = MOTOR_DRIVER_PRV_ADC_REF_VOLTAGE / MOTOR_MCU_CFG_AD12BIT_DATA;

    p_st_driver->ADCDataGet             = p_st_driver_cfg->ADCDataGet;
    p_st_driver->BLDCDutySet            = p_st_driver_cfg->BLDCDutySet;
    p_st_driver->BLDCZeroDutySet        = p_st_driver_cfg->BLDCZeroDutySet;
    p_st_driver->BLDCCompareDutySet     = p_st_driver_cfg->BLDCCompareDutySet;
    p_st_driver->PWMOutputStart         = p_st_driver_cfg->PWMOutputStart;
    p_st_driver->PWMOutputStop          = p_st_driver_cfg->PWMOutputStop;
    p_st_driver->f4_pwm_period_cnt      = p_st_driver_cfg->f4_pwm_period_cnt;
    p_st_driver->f4_pwm_dead_time_cnt   = p_st_driver_cfg->f4_pwm_dead_time_cnt;
    p_st_driver->f4_ad_crnt_per_digit   = f4_volt_per_digit /
                                            (p_st_driver_cfg->f4_crnt_amp_gain * p_st_driver_cfg->f4_shunt_ohm);
    p_st_driver->f4_ad_vdc_per_digit    = f4_volt_per_digit * p_st_driver_cfg->f4_volt_gain;
} /* End of function R_MOTOR_DRIVER_ParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcAnalogGet
* Description   : Get the current and bus voltage
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 p_f4_iu_ad  - U phase current A/D conversion value (pointer)
*                 p_f4_iw_ad  - W phase current A/D conversion value (pointer)
*                 p_f4_vdc_ad - Bus voltage A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcAnalogGet(st_motor_driver_t * p_st_driver,
                                  float * p_f4_iu_ad,
								  float * p_f4_iv_ad,
                                  float * p_f4_iw_ad,
                                  float * p_f4_vdc_ad)
{
    r_mtr_adc_tb st_ad_data;

    p_st_driver->ADCDataGet(&st_ad_data);
    *p_f4_iu_ad  = -1.0f * (st_ad_data.u2_iu_ad - MOTOR_MCU_CFG_ADC_OFFSET) * p_st_driver->f4_ad_crnt_per_digit;
    *p_f4_iv_ad  = -1.0f * (st_ad_data.u2_iv_ad - MOTOR_MCU_CFG_ADC_OFFSET) * p_st_driver->f4_ad_crnt_per_digit;
    *p_f4_iw_ad  = -1.0f * (st_ad_data.u2_iw_ad - MOTOR_MCU_CFG_ADC_OFFSET) * p_st_driver->f4_ad_crnt_per_digit;
    *p_f4_vdc_ad = st_ad_data.u2_vdc_ad * p_st_driver->f4_ad_vdc_per_digit;
} /* End of function R_MOTOR_DRIVER_BldcAnalogGet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcDutySet
* Description   : PWM duty setting
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 f4_duty_u   - The duty cycle of Phase-U (0.0 - 1.0);
*                 f4_duty_v   - The duty cycle of Phase-V (0.0 - 1.0);
*                 f4_duty_w   - The duty cycle of Phase-W (0.0 - 1.0);
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcDutySet(st_motor_driver_t * p_st_driver, float f4_duty_u, float f4_duty_v, float f4_duty_w)
{
    float f4_dead_time_cnt   = p_st_driver->f4_pwm_dead_time_cnt;
    float f4_period_time_cnt = p_st_driver->f4_pwm_period_cnt;
    uint16_t u2_count_u;
    uint16_t u2_count_v;
    uint16_t u2_count_w;

    u2_count_u = (uint16_t)((f4_period_time_cnt * (1.0f - f4_duty_u)) + (f4_dead_time_cnt * 0.5f));
    u2_count_v = (uint16_t)((f4_period_time_cnt * (1.0f - f4_duty_v)) + (f4_dead_time_cnt * 0.5f));
    u2_count_w = (uint16_t)((f4_period_time_cnt * (1.0f - f4_duty_w)) + (f4_dead_time_cnt * 0.5f));

    p_st_driver->BLDCDutySet(u2_count_u, u2_count_v, u2_count_w);
} /* End of function R_MOTOR_DRIVER_BldcDutySet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcZeroDutySet
* Description   : PWM zero duty setting
* Arguments     : p_st_driver - The pointer to the motor driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcZeroDutySet(st_motor_driver_t * p_st_driver)
{
    p_st_driver->BLDCZeroDutySet();
} /* End of function R_MOTOR_DRIVER_BldcZeroDutySet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcCompareDutySet
* Description   : PWM compare duty setting
* Arguments     : p_st_driver - The pointer to the motor driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcCompareDutySet(st_motor_driver_t * p_st_driver)
{
    p_st_driver->BLDCCompareDutySet();
} /* End of function R_MOTOR_DRIVER_BldcCompareDutySet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_PWMControlStop
* Description   : Stop PWM outputs
* Arguments     : p_st_driver - The pointer to the motor driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_PWMControlStop(st_motor_driver_t * p_st_driver)
{
    p_st_driver->PWMOutputStop();
} /* End of function R_MOTOR_DRIVER_PWMControlStop */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_PWMControlStart
* Description   : Start PWM outputs
* Arguments     : p_st_driver - The pointer to the motor driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_PWMControlStart(st_motor_driver_t * p_st_driver)
{
    p_st_driver->PWMOutputStart();
} /* End of function R_MOTOR_DRIVER_PWMControlStart */
