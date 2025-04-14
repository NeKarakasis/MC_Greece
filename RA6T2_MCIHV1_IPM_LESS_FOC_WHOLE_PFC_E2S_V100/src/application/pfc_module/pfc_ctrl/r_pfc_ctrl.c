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
* File Name   : r_pfc_ctrl.c
* Description : The PFC control module
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.02.2024 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Standard library headers */
#include <stdint.h>
#include <math.h>

/* Project headers */
#include "r_pfc_ctrl.h"

/***********************************************************************************************************************
* Function Name : pfc_ctrl_power_limit
* Description   : Calculate the limit value of PFC output power according to the input voltage effective value
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
*                 f4_vrms       - The effective value of the input voltage
* Return Value  : Power limit value
***********************************************************************************************************************/
float pfc_ctrl_power_limit(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_vrms)
{
    float f4_power_limit;

    f4_power_limit = f4_vrms * p_st_pfc_ctrl->f4_power_slope;

    if (f4_power_limit > p_st_pfc_ctrl->f4_power_max_limit)
    {
        f4_power_limit = p_st_pfc_ctrl->f4_power_max_limit;
    }
    else if (f4_power_limit < p_st_pfc_ctrl->f4_power_min_limit)
    {
        f4_power_limit = p_st_pfc_ctrl->f4_power_min_limit;
    }
    else
    {
        /* Do nothing */
    }

    /* return power limit */
    return (f4_power_limit);
} /* End of function pfc_ctrl_power_limit */

/***********************************************************************************************************************
* Function Name : pfc_ctrl_voltage_pi_control
* Description   : Voltage PI control
*                 Calculates the PFC output power from bus voltage command and actual bus voltage
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
* Return Value  : The reference value of PFC output power
***********************************************************************************************************************/
float pfc_ctrl_voltage_pi_control(st_pfc_ctrl_t *p_st_pfc_ctrl)
{
    float f4_power_ref_calc;

    p_st_pfc_ctrl->st_pi_voltage.f4_err = p_st_pfc_ctrl->f4_ref_vdc_ctrl - p_st_pfc_ctrl->f4_vdc_ad;
    f4_power_ref_calc = pfc_pi_ctrl(&p_st_pfc_ctrl->st_pi_voltage);

    if (f4_power_ref_calc < 0.0f)
    {
        f4_power_ref_calc = 0.0f;
    }

    return (f4_power_ref_calc);
} /* End of function pfc_ctrl_voltage_pi_control */

/***********************************************************************************************************************
* Function Name : pfc_ctrl_avr_anti_windup_control
* Description   : Anti-windup control for the automatic voltage regulator
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
*                 f4_pi_out     - The output of the voltage PI control
*                 f4_max        - The upper limit value of PI control output
*                 f4_min        - The lower limit value of PI control output
* Return Value  : None
***********************************************************************************************************************/
void pfc_ctrl_avr_anti_windup_control(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out, float f4_max, float f4_min)
{
    float f4_avr_p;
    float f4_avr_i;

    f4_avr_p = pfc_pi_ctrl_proportion_get(&p_st_pfc_ctrl->st_pi_voltage);
    f4_avr_i = pfc_pi_ctrl_integrator_get(&p_st_pfc_ctrl->st_pi_voltage);

    if (f4_avr_p >= f4_max)
    {
        f4_avr_p = f4_max;
    }
    else if (f4_avr_p <= f4_min)
    {
        f4_avr_p = f4_min;
    }
    else
    {
        /* Do nothing */
    }

    if ((f4_pi_out >= f4_max) && (f4_avr_i > f4_min))
    {
        f4_avr_i = f4_max - f4_avr_p;
        pfc_pi_ctrl_integrator_set(&p_st_pfc_ctrl->st_pi_voltage, f4_avr_i);
    }
    else if ((f4_pi_out <= f4_min) && (f4_avr_i < f4_min))
    {
        f4_avr_i = f4_min - f4_avr_p;
        pfc_pi_ctrl_integrator_set(&p_st_pfc_ctrl->st_pi_voltage, f4_avr_i);
    }
    else
    {
        /* Do nothing */
    }
} /* End of function pfc_ctrl_avr_anti_windup_control */

/***********************************************************************************************************************
* Function Name : pfc_ctrl_avr_ff_comp
* Description   : Voltage feed-forward control
*                 Calculate PFC current with PFC output power and voltage feed-forward compensation coefficient
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
*                 f4_pi_out     - The output of the voltage PI control
* Return Value  : The reference value of PFC current
***********************************************************************************************************************/
float pfc_ctrl_avr_ff_comp(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out)
{
    float f4_temp0;
    float f4_ff_comp_coef;
    float f4_iref_out;

    /* Under limit */
    if (p_st_pfc_ctrl->f4_vac_rms < p_st_pfc_ctrl->f4_avr_ff_comp_min_limit)
    {
        f4_temp0 = p_st_pfc_ctrl->f4_avr_ff_comp_min_limit;
    }
    else
    {
        f4_temp0 = p_st_pfc_ctrl->f4_vac_rms;
    }

    f4_ff_comp_coef = p_st_pfc_ctrl->f4_vac_ad / (f4_temp0 * f4_temp0);    /* coef = Vac / (Vrms * Vrms) */
    f4_iref_out = f4_pi_out * f4_ff_comp_coef;                             /* Iref = PIout * coef */

    return (f4_iref_out);
} /* End of function pfc_ctrl_avr_ff_comp */

/***********************************************************************************************************************
* Function Name : pfc_ctrl_current_pi_control
* Description   : Current PI control
*                 Calculates the duty ratio from PFC current command and actual PFC current
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
* Return Value  : Duty ratio
***********************************************************************************************************************/
float pfc_ctrl_current_pi_control(st_pfc_ctrl_t *p_st_pfc_ctrl)
{
    float f4_duty_ref_calc;

    p_st_pfc_ctrl->st_pi_current.f4_err = p_st_pfc_ctrl->f4_ref_ichp_ctrl - p_st_pfc_ctrl->f4_ichp_ad;
    f4_duty_ref_calc = pfc_pi_ctrl(&p_st_pfc_ctrl->st_pi_current);

    return (f4_duty_ref_calc);
} /* End of function pfc_ctrl_current_pi_control */

/***********************************************************************************************************************
* Function Name : pfc_ctrl_acr_ff_comp
* Description   : Duty ratio feed-forward control
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
*                 f4_pi_out     - The output of the current PI control
* Return Value  : Compensated duty ratio
***********************************************************************************************************************/
float pfc_ctrl_acr_ff_comp(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out)
{
    float f4_temp0;
    float f4_duty_ff;
    float f4_duty_ff_comp_gain;
    float f4_duty_out;

    /* Under limit */
    if (p_st_pfc_ctrl->f4_vdc_ad < p_st_pfc_ctrl->f4_acr_ff_comp_min_limit)
    {
        f4_temp0 = p_st_pfc_ctrl->f4_acr_ff_comp_min_limit;
    }
    else
    {
        f4_temp0 = p_st_pfc_ctrl->f4_vdc_ad;
    }

    /* Calculate duty ratio feed-forward value */
    f4_duty_ff = 1.0f - (p_st_pfc_ctrl->f4_vac_ad / f4_temp0);    /* duty_ff = 1.0 - (Vac / Vdc) */

    /* Calculate duty ratio feed-forward compensation gain */
    f4_duty_ff_comp_gain = (float)p_st_pfc_ctrl->u2_duty_ff_softstart_cnt * p_st_pfc_ctrl->f4_duty_ff_softstart_up_step;

    p_st_pfc_ctrl->u2_duty_ff_softstart_cnt++;
    if (p_st_pfc_ctrl->u2_duty_ff_softstart_cnt >= p_st_pfc_ctrl->u2_duty_ff_softstart_max_cnt)
    {
        p_st_pfc_ctrl->u2_duty_ff_softstart_cnt = p_st_pfc_ctrl->u2_duty_ff_softstart_max_cnt;
    }

    /* Soft start */
    p_st_pfc_ctrl->f4_duty_ff_comp = f4_duty_ff * f4_duty_ff_comp_gain * p_st_pfc_ctrl->f4_duty_ff_comp_coef;

    f4_duty_out = f4_pi_out + p_st_pfc_ctrl->f4_duty_ff_comp;

    return (f4_duty_out);
} /* End of function pfc_ctrl_acr_ff_comp */

/***********************************************************************************************************************
* Function Name : pfc_ctrl_acr_anti_windup_control
* Description   : Anti-windup control for the automatic current regulator
* Arguments     : p_st_pfc_ctrl - The pointer to the PFC control parameter structure
*                 f4_pi_out     - The output of the current PI control
*                 f4_max        - The upper limit value of PI control output
*                 f4_min        - The lower limit value of PI control output
* Return Value  : None
***********************************************************************************************************************/
void pfc_ctrl_acr_anti_windup_control(st_pfc_ctrl_t *p_st_pfc_ctrl, float f4_pi_out, float f4_max, float f4_min)
{
    float f4_avr_p;
    float f4_avr_i;

    f4_avr_p = pfc_pi_ctrl_proportion_get(&p_st_pfc_ctrl->st_pi_current);
    f4_avr_p = f4_avr_p + p_st_pfc_ctrl->f4_duty_ff_comp;
    f4_avr_i = pfc_pi_ctrl_integrator_get(&p_st_pfc_ctrl->st_pi_current);

    if (f4_avr_p >= f4_max)
    {
        f4_avr_p = f4_max;
    }
    else if (f4_avr_p <= f4_min)
    {
        f4_avr_p = f4_min;
    }
    else
    {
        /* Do nothing */
    }

    if ((f4_pi_out >= f4_max) && (f4_avr_i > 0.0f))
    {
        f4_avr_i = f4_max - f4_avr_p;
        pfc_pi_ctrl_integrator_set(&p_st_pfc_ctrl->st_pi_current, f4_avr_i);
    }
    else
    {
        /* Do nothing */
    }
} /* End of function pfc_ctrl_acr_anti_windup_control */
