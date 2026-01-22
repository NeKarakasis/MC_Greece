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
* File Name   : r_pfc_driver.c
* Description : The processes of PFC driver module
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/
/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <math.h>
#include "r_pfc_driver.h"
#include "r_pfc_cfg.h"

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#if defined(PFC_CFG_ADC_REF_VOLTAGE)
#define PFC_DRIVER_PRV_ADC_REF_VOLTAGE          (PFC_CFG_ADC_REF_VOLTAGE) /* AD reference voltage */
#else
#define PFC_DRIVER_PRV_ADC_REF_VOLTAGE          (3.3f)                    /* Default AD reference voltage */
#endif

/***********************************************************************************************************************
* Exported global variables
***********************************************************************************************************************/
st_pfc_driver_t g_st_pfc_driver;

/***********************************************************************************************************************
* Global functions definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_Open
* Description   : Open PFC driver module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_Open(void)
{
    st_pfc_driver_t * p_st_pfc_driver;

    p_st_pfc_driver = &g_st_pfc_driver;

    p_st_pfc_driver->f4_ad_vac_per_digit  = 0.0f;
    p_st_pfc_driver->f4_ad_vdc_per_digit  = 0.0f;
    p_st_pfc_driver->f4_ad_crnt_per_digit = 0.0f;
    p_st_pfc_driver->f4_pwm_period_cnt    = 0.0f;

} /* End of function R_PFC_DRIVER_Open */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_Close
* Description   : Close PFC driver module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_Close(void)
{
    /* No structure member need to be reset in Close function */
} /* End of function R_PFC_DRIVER_Close */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_ParameterUpdate
* Description   : Update the PFC driver parameter
* Arguments     : p_st_pfc_driver     - The pointer to the PFC driver structure
*                 p_st_pfc_driver_cfg - The pointer to the PFC driver configuration parameter structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_ParameterUpdate(st_pfc_driver_t * p_st_pfc_driver,
                                  const st_pfc_driver_cfg_t * p_st_pfc_driver_cfg)
{
    float f4_volt_per_digit;

    f4_volt_per_digit = PFC_DRIVER_PRV_ADC_REF_VOLTAGE / PFC_MCU_CFG_AD12BIT_DATA;

    p_st_pfc_driver->ADCDataGet           = p_st_pfc_driver_cfg->ADCDataGet;
    p_st_pfc_driver->DutySet              = p_st_pfc_driver_cfg->DutySet;
    p_st_pfc_driver->PWMOutputStart       = p_st_pfc_driver_cfg->PWMOutputStart;
    p_st_pfc_driver->PWMOutputStop        = p_st_pfc_driver_cfg->PWMOutputStop;
    p_st_pfc_driver->RelayOn              = p_st_pfc_driver_cfg->RelayOn;
    p_st_pfc_driver->RelayOff             = p_st_pfc_driver_cfg->RelayOff;
    p_st_pfc_driver->OverheatCheck        = p_st_pfc_driver_cfg->OverheatCheck;
    p_st_pfc_driver->f4_pwm_period_cnt    = p_st_pfc_driver_cfg->f4_pwm_period_cnt;
    p_st_pfc_driver->u2_pwm_valid_max_cnt = p_st_pfc_driver_cfg->u2_pwm_valid_max_cnt;
    p_st_pfc_driver->u2_pwm_valid_min_cnt = p_st_pfc_driver_cfg->u2_pwm_valid_min_cnt;
    p_st_pfc_driver->f4_ad_vac_per_digit  = f4_volt_per_digit * p_st_pfc_driver_cfg->f4_ac_volt_gain;
    p_st_pfc_driver->f4_ad_vdc_per_digit  = f4_volt_per_digit * p_st_pfc_driver_cfg->f4_bus_volt_gain;
    p_st_pfc_driver->f4_ad_crnt_per_digit = f4_volt_per_digit /
                                            (p_st_pfc_driver_cfg->f4_crnt_amp_gain * p_st_pfc_driver_cfg->f4_shunt_ohm);
    p_st_pfc_driver->s2_input_volt_offset = p_st_pfc_driver_cfg->s2_input_volt_offset;
    p_st_pfc_driver->s2_bus_volt_offset   = p_st_pfc_driver_cfg->s2_bus_volt_offset;
    p_st_pfc_driver->s2_pfc_crnt_offset   = p_st_pfc_driver_cfg->s2_pfc_crnt_offset;
} /* End of function R_PFC_DRIVER_ParameterUpdate */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_AnalogGet
* Description   : Get the input AC voltage, bus voltage, and PFC current
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
*                 p_f4_vac_ad     - The AC voltage A/D conversion value (pointer)
*                 p_f4_vdc_ad     - The bus voltage A/D conversion value (pointer)
*                 p_f4_ichp_ad    - The PFC current A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_AnalogGet(st_pfc_driver_t * p_st_pfc_driver,
                            float * p_f4_vac_ad,
                            float * p_f4_vdc_ad,
                            float * p_f4_ichp_ad)
{
    r_pfc_adc_tb st_ad_data;
    float f4_vac;
    float f4_vdc;
    float f4_ichp;

    p_st_pfc_driver->ADCDataGet(&st_ad_data);

    f4_vac  = (float)(st_ad_data.u2_vac_ad - PFC_MCU_CFG_ADC_OFFSET - p_st_pfc_driver->s2_input_volt_offset) *
                      p_st_pfc_driver->f4_ad_vac_per_digit;
    f4_vdc  = (float)(st_ad_data.u2_vdc_ad - p_st_pfc_driver->s2_bus_volt_offset) *
                      p_st_pfc_driver->f4_ad_vdc_per_digit;
    f4_ichp = (float)(st_ad_data.u2_ichp_ad - PFC_MCU_CFG_ADC_OFFSET - p_st_pfc_driver->s2_pfc_crnt_offset) *
                      p_st_pfc_driver->f4_ad_crnt_per_digit;

    *p_f4_vac_ad  = fabsf(f4_vac);
    *p_f4_vdc_ad  = fabsf(f4_vdc);
    *p_f4_ichp_ad = fabsf(f4_ichp);
} /* End of function R_PFC_DRIVER_AnalogGet */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_DutySet
* Description   : PWM duty setting
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
*                 f4_duty         - The duty cycle for the PFC control
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_DutySet(st_pfc_driver_t * p_st_pfc_driver, float f4_duty)
{
    float f4_period_time_cnt = p_st_pfc_driver->f4_pwm_period_cnt;
    uint16_t u2_count;

    u2_count = (uint16_t)(f4_period_time_cnt * f4_duty);

    if (u2_count > p_st_pfc_driver->u2_pwm_valid_max_cnt)
    {
        u2_count = p_st_pfc_driver->u2_pwm_valid_max_cnt;
    }
    else if (u2_count < p_st_pfc_driver->u2_pwm_valid_min_cnt)
    {
        u2_count = p_st_pfc_driver->u2_pwm_valid_min_cnt;
    }
    else
    {
        /* Do nothing  */
    }

    p_st_pfc_driver->DutySet(u2_count);
} /* End of function R_PFC_DRIVER_DutySet */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_PWMControlStop
* Description   : Stop PWM outputs
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_PWMControlStop(st_pfc_driver_t * p_st_pfc_driver)
{
    p_st_pfc_driver->PWMOutputStop();
} /* End of function R_PFC_DRIVER_PWMControlStop */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_PWMControlStart
* Description   : Start PWM outputs
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_PWMControlStart(st_pfc_driver_t * p_st_pfc_driver)
{
    p_st_pfc_driver->PWMOutputStart();
} /* End of function R_PFC_DRIVER_PWMControlStart */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_RelayOn
* Description   : Relay ON
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_RelayOn(st_pfc_driver_t * p_st_pfc_driver)
{
    p_st_pfc_driver->RelayOn();
} /* End of function R_PFC_DRIVER_RelayOn */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_RelayOff
* Description   : Relay OFF
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
* Return Value  : None
***********************************************************************************************************************/
void R_PFC_DRIVER_RelayOff(st_pfc_driver_t * p_st_pfc_driver)
{
    p_st_pfc_driver->RelayOff();
} /* End of function R_PFC_DRIVER_RelayOff */

/***********************************************************************************************************************
* Function Name : R_PFC_DRIVER_OverheatCheck
* Description   : Get the port level for over heating error detection
* Arguments     : p_st_pfc_driver - The pointer to the PFC driver structure
* Return Value  : Port level
***********************************************************************************************************************/
uint8_t R_PFC_DRIVER_OverheatCheck(st_pfc_driver_t * p_st_pfc_driver)
{
    uint8_t u1_temp0;

    u1_temp0 = p_st_pfc_driver->OverheatCheck();     /* Get port level */

    return (u1_temp0);
} /* End of function R_PFC_DRIVER_OverheatCheck */
