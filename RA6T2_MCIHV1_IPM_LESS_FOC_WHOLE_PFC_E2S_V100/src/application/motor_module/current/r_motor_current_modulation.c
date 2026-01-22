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
* File Name    : r_motor_current_modulation.c
* Description  : This module convert voltages to duties
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_motor_module_cfg.h"
#include "r_motor_current_modulation.h"

#if defined (MOTOR_TYPE_BLDC)
#if (CURRENT_CFG_MODULATION_METHOD == MOD_METHOD_SVPWM)
/***********************************************************************************************************************
* Function Name: motor_current_svpwm_mod_start
* Description  : Space vector modulation
* Arguments    : p_f4_v_in  - Input data, in an array [Vu,Vv,Vw] (pointer)
*                p_f4_v_out - Where to store output data, in an array [Vu,Vv,Vw] (pointer)
* Return Value : None
***********************************************************************************************************************/
static inline void motor_current_svpwm_mod_start(const float *p_f4_v_in, float *p_f4_v_out)
{
    float f4_v_max;
    float f4_v_min;
    float f4_v_com;

    /* Sort vu vv vw */
    if (p_f4_v_in[0] > p_f4_v_in[1])
    {
        f4_v_max = p_f4_v_in[0];
        f4_v_min = p_f4_v_in[1];
    }
    else
    {
        f4_v_max = p_f4_v_in[1];
        f4_v_min = p_f4_v_in[0];
    }

    /* Vcom = (Vmin + Vmax)/2 */
    if (p_f4_v_in[2] > f4_v_max)
    {
        f4_v_com = (p_f4_v_in[2] + f4_v_min) * 0.5f;
    }
    else if (p_f4_v_in[2] < f4_v_min)
    {
        f4_v_com = (f4_v_max + p_f4_v_in[2]) * 0.5f;
    }
    else
    {
        f4_v_com = (f4_v_max + f4_v_min) * 0.5f;
    }

    p_f4_v_out[0] = p_f4_v_in[0] - f4_v_com;
    p_f4_v_out[1] = p_f4_v_in[1] - f4_v_com;
    p_f4_v_out[2] = p_f4_v_in[2] - f4_v_com;
} /* End of function motor_current_svpwm_mod_start() */
#endif
#endif

/***********************************************************************************************************************
* Function Name: motor_current_mod_limit_set
* Description  : Limits the duty cycle, and detect saturation (if function enabled)
* Arguments    : p_st_mod -   Pointer to the modulation data structure
*                p_f4_duty -  Pointer to the input and output duty cycle array, array length = 3
* Return Value : None
***********************************************************************************************************************/
static inline void motor_current_mod_limit_set(st_mod_t *p_st_mod, float *p_f4_duty)
{
    float f4_min_duty = p_st_mod->f4_min_duty;
    float f4_max_duty = p_st_mod->f4_max_duty;

#if defined(MOTOR_TYPE_BLDC)
    if (p_f4_duty[0] > f4_max_duty)
    {
        p_f4_duty[0] = f4_max_duty;
    }
    else if (p_f4_duty[0] < f4_min_duty)
    {
        p_f4_duty[0] = f4_min_duty;
    }
    else
    {
        /* Do nothing  */
    }
    if (p_f4_duty[1] > f4_max_duty)
    {
        p_f4_duty[1] = f4_max_duty;
    }
    else if (p_f4_duty[1] < f4_min_duty)
    {
        p_f4_duty[1] = f4_min_duty;
    }
    else
    {
        /* Do nothing  */
    }
    if (p_f4_duty[2] > f4_max_duty)
    {
        p_f4_duty[2] = f4_max_duty;
    }
    else if (p_f4_duty[2] < f4_min_duty)
    {
        p_f4_duty[2] = f4_min_duty;
    }
    else
    {
        /* Do nothing  */
    }
#elif defined(MOTOR_TYPE_STM)

    if (p_f4_duty[0] > f4_max_duty)
    {
        p_f4_duty[0] = f4_max_duty;
    }
    else if (p_f4_duty[0] < f4_min_duty)
    {
        p_f4_duty[0] = f4_min_duty;
    }
    else
    {
        /* Do nothing */
    }
    if (p_f4_duty[1] > f4_max_duty)
    {
        p_f4_duty[1] = f4_max_duty;
    }
    else if (p_f4_duty[1] < f4_min_duty)
    {
        p_f4_duty[1] = f4_min_duty;
    }
    else
    {
        /* Do nothing */
    }

#endif
}/* End of function motor_current_mod_limit_set() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_init
* Description  : Initializes modulation module
* Arguments    : p_st_mod - Pointer to the modulation data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_init(st_mod_t *p_st_mod)
{
    if (p_st_mod)
    {
        p_st_mod->f4_1_div_vdc = 1.0f;                      /* Default value */
        p_st_mod->f4_max_duty = MOD_DEFAULT_MAX_DUTY;
        p_st_mod->f4_min_duty = 0.0f;
        p_st_mod->f4_neutral_duty = MOD_DEFAULT_MAX_DUTY * 0.5f;
        p_st_mod->f4_voltage_error_ratio = 0.0f;
        motor_current_mod_reset(p_st_mod);
        motor_current_mod_vdc_set(p_st_mod, INVERTER_CFG_INPUT_V);
        motor_current_mod_max_duty_set(p_st_mod, MOD_DEFAULT_MAX_DUTY);
        motor_current_mod_min_duty_set(p_st_mod, 0.0f);
        motor_current_mod_volt_err_ratio_set(p_st_mod, 0.0f);
    }

} /* End of function motor_current_mod_init() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_reset
* Description  : Resets modulation module
* Arguments    : p_st_mod - Pointer to the modulation data structure to be initialized
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_reset(st_mod_t *p_st_mod)
{
    (void)p_st_mod; /* This line is to avoid Warning due to unused parameter  */
    /* Do nothing  */
} /* End of function motor_current_mod_reset() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_start
* Description  : Calculates duty cycle from input 2-phase or 3-phase voltage (bipolar)
* Arguments    : p_st_mod  - Pointer to the modulation data structure
*              : p_f4_v_in - Pointer to the 2-phase(STM),3-phase(BLDC) input voltage
*              : p_f4_duty_out - Where to store the 2-phase(STM),3-phase(BLDC) output duty cycle
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_start(st_mod_t *p_st_mod, const float *p_f4_v_in, float *p_f4_duty_out)
{

#if defined(MOTOR_TYPE_BLDC)
    float f4_v_out[3];
#endif

    if ((0 != p_st_mod) && (0 != p_f4_duty_out) && (0 != p_f4_v_in))
    {
#if defined(MOTOR_TYPE_BLDC)
#if (CURRENT_CFG_MODULATION_METHOD == MOD_METHOD_SVPWM)
        motor_current_svpwm_mod_start(p_f4_v_in, f4_v_out);
#elif (CURRENT_CFG_MODULATION_METHOD == MOD_METHOD_SPWM)
        f4_v_out[0] = p_f4_v_in[0];
        f4_v_out[1] = p_f4_v_in[1];
        f4_v_out[2] = p_f4_v_in[2];
#endif

        p_f4_duty_out[0] = (f4_v_out[0] * p_st_mod->f4_1_div_vdc) + p_st_mod->f4_neutral_duty;
        p_f4_duty_out[1] = (f4_v_out[1] * p_st_mod->f4_1_div_vdc) + p_st_mod->f4_neutral_duty;
        p_f4_duty_out[2] = (f4_v_out[2] * p_st_mod->f4_1_div_vdc) + p_st_mod->f4_neutral_duty;

#elif defined(MOTOR_TYPE_STM)
        p_f4_duty_out[0] = ((p_f4_v_in[0] * p_st_mod->f4_1_div_vdc) * 0.5f) + 0.5f;
        p_f4_duty_out[1] = ((p_f4_v_in[1] * p_st_mod->f4_1_div_vdc) * 0.5f) + 0.5f;
#endif
        motor_current_mod_limit_set(p_st_mod, p_f4_duty_out);
    }

} /* End of function motor_current_mod_start() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_vdc_set
* Description  : Sets the DC bus voltage used in modulation
* Arguments    : p_st_mod - Pointer to the modulation data structure
*                f4_vdc   - The DC bus voltage to be used in modulation
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_vdc_set(st_mod_t *p_st_mod, float f4_vdc)
{
    if (f4_vdc > 0.0f)
    {
        p_st_mod->f4_vdc        = f4_vdc;
        p_st_mod->f4_1_div_vdc  = 1.0f / f4_vdc;
    }
} /* End of function motor_current_mod_vdc_set() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_max_duty_set
* Description  : Sets the maximum duty cycle
* Arguments    : p_st_mod - Pointer to the modulation data structure
*                f4_max_duty - Maximum duty cycle to set
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_max_duty_set(st_mod_t *p_st_mod, float f4_max_duty)
{
    if ((0 != p_st_mod) && ((f4_max_duty > 0.0f) && (f4_max_duty <= 1.0f)))
    {
        p_st_mod->f4_max_duty       = f4_max_duty;
        p_st_mod->f4_neutral_duty   = (p_st_mod->f4_max_duty + p_st_mod->f4_min_duty) * 0.5f;
    }
} /* End of function motor_current_mod_max_duty_set() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_min_duty_set
* Description  : Sets the minimum duty cycle
* Arguments    : p_st_mod - Pointer to the modulation data structure
*                f4_max_duty - Minimum duty cycle to set
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_min_duty_set(st_mod_t *p_st_mod, float f4_min_duty)
{
    if ((0 != p_st_mod) && (f4_min_duty > 0.0f))
    {
        p_st_mod->f4_min_duty       = f4_min_duty;
        p_st_mod->f4_neutral_duty   = (p_st_mod->f4_max_duty + p_st_mod->f4_min_duty) * 0.5f;
    }
} /* End of function motor_current_mod_min_duty_set() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_volt_err_ratio_set
* Description  : Sets the voltage error ratio (VoltageError/Vdc)
* Arguments    : p_st_mod - Pointer to the modulation data structure
*                f4_voltage_error_ratio - The voltage error ratio
* Return Value : None
***********************************************************************************************************************/
void motor_current_mod_volt_err_ratio_set(st_mod_t *p_st_mod, float f4_voltage_error_ratio)
{
    p_st_mod->f4_voltage_error_ratio = f4_voltage_error_ratio;
} /* End of function motor_current_mod_volt_err_ratio_set() */

/***********************************************************************************************************************
* Function Name: motor_current_mod_voltage_multiplier_get
* Description  : Gets the voltage multiplier
* Arguments    : p_st_mod - Pointer to the modulation data structure
* Return Value : Voltage multiplier
***********************************************************************************************************************/
float motor_current_mod_voltage_multiplier_get(st_mod_t *p_st_mod)
{
    float f4_usable_duty_cycle;

    f4_usable_duty_cycle = (p_st_mod->f4_max_duty - p_st_mod->f4_min_duty) - (2.0f * p_st_mod->f4_voltage_error_ratio);

#if defined(MOTOR_TYPE_BLDC)
#if (CURRENT_CFG_MODULATION_METHOD == MOD_METHOD_SPWM)
    return (f4_usable_duty_cycle);
#elif (CURRENT_CFG_MODULATION_METHOD == MOD_METHOD_SVPWM)
    return (f4_usable_duty_cycle * MOD_SVPWM_MULT);
#endif
#elif defined(MOTOR_TYPE_STM)
    return (f4_usable_duty_cycle);
#endif
} /* End of function motor_current_mod_voltage_multiplier_get */

/***********************************************************************************************************************
* Function Name: motor_current_mod_max_va_get
* Description  : Gets the maximum magnitude of voltage vector
* Arguments    : p_st_mod - The pointer to the modulation data structure
* Return Value : The maximum magnitude of voltage vector
***********************************************************************************************************************/
float motor_current_mod_max_va_get(st_mod_t *p_st_mod)
{
#if defined(MOTOR_TYPE_BLDC)
    return ((MOD_VDC_TO_VAMAX_MULT * p_st_mod->f4_vdc) * motor_current_mod_voltage_multiplier_get(p_st_mod));
#elif defined(MOTOR_TYPE_STM)
    return ((p_st_mod->f4_vdc) * motor_current_mod_voltage_multiplier_get(p_st_mod));
#endif
} /* End of function motor_current_mod_max_va_get */
