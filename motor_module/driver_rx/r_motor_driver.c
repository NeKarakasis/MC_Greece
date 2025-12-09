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
*         : 07.12.2021 1.00
*         : 12.05.2022 1.10
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
void R_MOTOR_DRIVER_ParameterUpdate(st_motor_driver_t * p_st_driver,
                                    const st_motor_driver_cfg_t * p_st_driver_cfg)
{
    float f4_volt_per_digit;

    f4_volt_per_digit = MOTOR_DRIVER_PRV_ADC_REF_VOLTAGE / MOTOR_MCU_CFG_AD12BIT_DATA;

    p_st_driver->ADCDataGet           = p_st_driver_cfg->ADCDataGet;
#if defined(MOTOR_SHUNT_TYPE_2_SHUNT)
    p_st_driver->BLDCDutySet          = p_st_driver_cfg->BLDCDutySet;
#endif
    p_st_driver->PWMOutputStart       = p_st_driver_cfg->PWMOutputStart;
    p_st_driver->PWMOutputStop        = p_st_driver_cfg->PWMOutputStop;
    p_st_driver->f4_pwm_period_cnt    = p_st_driver_cfg->f4_pwm_period_cnt;
    p_st_driver->f4_pwm_dead_time_cnt = p_st_driver_cfg->f4_pwm_dead_time_cnt;
    p_st_driver->f4_ad_crnt_per_digit = f4_volt_per_digit /
                                        (p_st_driver_cfg->f4_crnt_amp_gain);//* p_st_driver_cfg->f4_shunt_ohm);
    p_st_driver->f4_ad_vdc_per_digit  = f4_volt_per_digit * p_st_driver_cfg->f4_volt_gain;
} /* End of function R_MOTOR_DRIVER_ParameterUpdate */

#if defined(MOTOR_SHUNT_TYPE_2_SHUNT)
uint16_t iu_adc_value;
uint16_t iv_adc_value;
uint16_t iw_adc_value;
uint16_t vdc_adc_value;

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcAnalogGet
* Description   : Get the current and bus voltage
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 p_f4_iu_ad  - U phase current A/D conversion value (pointer)
*                 p_f4_iw_ad  - W phase current A/D conversion value (pointer)
*                 p_f4_vdc_ad - Bus voltage A/D conversion value (pointer)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcAnalogGet_old(st_motor_driver_t * p_st_driver,
                                  float * p_f4_iu_ad,
                                  float * p_f4_iv_ad,
								  float * p_f4_iw_ad,
                                  float * p_f4_vdc_ad)
{
    r_mtr_adc_tb st_ad_data;
    float f4_volt_per_digit;
    f4_volt_per_digit = MOTOR_DRIVER_PRV_ADC_REF_VOLTAGE / MOTOR_MCU_CFG_AD12BIT_DATA;
    p_st_driver->ADCDataGet(&st_ad_data);
    iu_adc_value = st_ad_data.u2_iu_ad;
    iv_adc_value = st_ad_data.u2_iv_ad;
    iw_adc_value = st_ad_data.u2_iw_ad;
    vdc_adc_value = st_ad_data.u2_vdc_ad;
    *p_f4_iu_ad = ( 0.639535f - ((st_ad_data.u2_iu_ad * f4_volt_per_digit) / 3.077f) ) * 2.293333f;
    *p_f4_iv_ad = ( 0.639535f - ((st_ad_data.u2_iv_ad * f4_volt_per_digit) / 3.077f)  ) * 2.293333f;
    *p_f4_iw_ad = ( 0.639535f - ((st_ad_data.u2_iw_ad * f4_volt_per_digit) / 3.077f) ) * 2.293333f;

    *p_f4_vdc_ad = st_ad_data.u2_vdc_ad * p_st_driver->f4_ad_vdc_per_digit;
} /* End of function R_MOTOR_DRIVER_BldcAnalogGet */

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_AdcRawGet
* Description   : Retrieves raw ADC data from the motor driver hardware using the provided driver structure.
* Arguments     : p_st_driver   - The pointer to the motor driver structure
*                 p_st_adc_raw  - The pointer to the structure where raw ADC values will be stored
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_AdcRawGet(st_motor_driver_t * p_st_driver, r_mtr_adc_tb * p_st_adc_raw)
{
    p_st_driver->ADCDataGet(p_st_adc_raw);
}

/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_AdcConvert
* Description   : Converts raw ADC readings of phase currents and bus voltage to floating-point physical values.
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 p_st_adc_raw - The pointer to the structure containing raw ADC values
*                 p_f4_iu     - Pointer to store converted U phase current (in Amps)
*                 p_f4_iv     - Pointer to store converted V phase current (in Amps)
*                 p_f4_iw     - Pointer to store converted W phase current (in Amps)
*                 p_f4_vdc    - Pointer to store converted bus voltage (in Volts)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_AdcConvert(st_motor_driver_t * p_st_driver,
                                r_mtr_adc_tb * p_st_adc_raw,
                                float * p_f4_iu,
                                float * p_f4_iv,
                                float * p_f4_iw,
                                float * p_f4_vdc)
{
    float f4_volt_per_digit = MOTOR_DRIVER_PRV_ADC_REF_VOLTAGE / MOTOR_MCU_CFG_AD12BIT_DATA;
    

    *p_f4_iu = ( 0.639535f - ((p_st_adc_raw->u2_iu_ad * f4_volt_per_digit) / 3.077f) ) * 2.293333f;
    *p_f4_iv = ( 0.639535f - ((p_st_adc_raw->u2_iv_ad * f4_volt_per_digit) / 3.077f)  ) * 2.293333f;
    *p_f4_iw = ( 0.639535f - ((p_st_adc_raw->u2_iw_ad * f4_volt_per_digit) / 3.077f) ) * 2.293333f;

    *p_f4_vdc = p_st_adc_raw->u2_vdc_ad * p_st_driver->f4_ad_vdc_per_digit;
}
/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcCurrentReconstruct
* Description   : Opposite logic to standard current reconstruction. Discards the phase with the lowest duty cycle
*                 (instead of the highest), under the assumption that low duty means less reliable current sampling.
*                 Useful for testing/validation purposes.
* Arguments     : f4_duty_u - PWM duty cycle for U phase (per unit, 0.0 to 1.0)
*                 f4_duty_v - PWM duty cycle for V phase (per unit, 0.0 to 1.0)
*                 f4_duty_w - PWM duty cycle for W phase (per unit, 0.0 to 1.0)
*                 iu_raw    - Converted U phase current value (float)
*                 iv_raw    - Converted V phase current value (float)
*                 iw_raw    - Converted W phase current value (float)
*                 p_iu      - Pointer to store final U phase current
*                 p_iv      - Pointer to store final V phase current
*                 p_iw      - Pointer to store final W phase current
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcCurrentReconstruct(float f4_duty_u,
                                                  float f4_duty_v,
                                                  float f4_duty_w,
                                                  float iu_raw,
                                                  float iv_raw,
                                                  float iw_raw,
                                                  float * p_iu,
                                                  float * p_iv,
                                                  float * p_iw)
{
    if (f4_duty_u >= f4_duty_v)
    {
        if (f4_duty_u >= f4_duty_w)
        {
            // U is min > discard IU
            *p_iv = iv_raw;
            *p_iw = iw_raw;
            *p_iu = -(*p_iv + *p_iw);
        }
        else
        {
            // W is min > discard IW
            *p_iu = iu_raw;
            *p_iv = iv_raw;
            *p_iw = -(*p_iu + *p_iv);
        }
    }
    else
    {
        if (f4_duty_v >= f4_duty_w)
        {
            // V is min > discard IV
            *p_iu = iu_raw;
            *p_iw = iw_raw;
            *p_iv = -(*p_iu + *p_iw);
        }
        else
        {
            // W is min > discard IW
            *p_iu = iu_raw;
            *p_iv = iv_raw;
            *p_iw = -(*p_iu + *p_iv);
        }
    }
}


/***********************************************************************************************************************
* Function Name : R_MOTOR_DRIVER_BldcAnalogGet
* Description   : Gets phase currents and bus voltage in physical units. It reads ADC values,
*                 converts them to real-world units, and selects the most reliable phase current
*                 samples based on per-unit PWM duty cycles. The third current is reconstructed
*                 using Kirchhoff's Current Law.
* Arguments     : p_st_driver - The pointer to the motor driver structure
*                 f4_duty_u   - PWM duty cycle for U phase (per unit, 0.0 to 1.0)
*                 f4_duty_v   - PWM duty cycle for V phase (per unit, 0.0 to 1.0)
*                 f4_duty_w   - PWM duty cycle for W phase (per unit, 0.0 to 1.0)
*                 p_f4_iu     - Pointer to store final U phase current (in Amps)
*                 p_f4_iv     - Pointer to store final V phase current (in Amps)
*                 p_f4_iw     - Pointer to store final W phase current (in Amps)
*                 p_f4_vdc    - Pointer to store bus voltage (in Volts)
* Return Value  : None
***********************************************************************************************************************/
void R_MOTOR_DRIVER_BldcAnalogGet(st_motor_driver_t * p_st_driver,
                                  float f4_duty_u,
                                  float f4_duty_v,
                                  float f4_duty_w,
                                  float * p_f4_iu,
                                  float * p_f4_iv,
                                  float * p_f4_iw,
								  float * p_raw_iu,
								  float * p_raw_iv,
								  float * p_raw_iw,
                                  float * p_f4_vdc)
{
    r_mtr_adc_tb adc_raw;
    float iu_raw, iv_raw, iw_raw;

    // Step 1: Get raw ADC readings
    R_MOTOR_DRIVER_AdcRawGet(p_st_driver, &adc_raw);

    // Step 2: Convert raw ADC values to physical units (Amps, Volts)
    R_MOTOR_DRIVER_AdcConvert(p_st_driver, &adc_raw, &iu_raw, &iv_raw, &iw_raw, p_f4_vdc);
    *p_raw_iu = iu_raw;
    *p_raw_iv = iv_raw;
    *p_raw_iw = iw_raw;
    // Step 3: Reconstruct phase currents using valid channels
    R_MOTOR_DRIVER_BldcCurrentReconstruct(f4_duty_u, f4_duty_v, f4_duty_w,
                                              iu_raw, iv_raw, iw_raw,
                                              p_f4_iu, p_f4_iv, p_f4_iw);
}



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
                                        float * p_f4_vdc_ad)
{
    r_mtr_adc_tb st_ad_data;

    p_st_driver->ADCDataGet(&st_ad_data);

    /* Get double buffer A/D data */
    p_f4_Iac_ad[0] = -(MOTOR_MCU_CFG_ADC_OFFSET - st_ad_data.u2_Iac_raw0) * p_st_driver->f4_ad_crnt_per_digit;
    p_f4_Iac_ad[1] = (MOTOR_MCU_CFG_ADC_OFFSET - st_ad_data.u2_Iac_raw1) * p_st_driver->f4_ad_crnt_per_digit;

    /* Get main line voltage */
    *p_f4_vdc_ad = st_ad_data.u2_vdc_ad * p_st_driver->f4_ad_vdc_per_digit;
} /* End of function R_MOTOR_DRIVER_1ShuntBldcAnalogGet */
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
#endif

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
