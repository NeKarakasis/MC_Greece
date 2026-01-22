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
* File Name   : r_motor_filter.c
* Description : Processes of filters
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include <math.h>
#include "r_motor_filter.h"
#include "r_motor_common.h"

/***********************************************************************************************************************
* Function Name : motor_filter_lpff
* Description   : LPF process
* Arguments     : f4_lpf_input      - LPF input value
                  f4_pre_lpf_output - Previous LPF output value
                  f4_lpf_k          - LPF parameter
* Return Value  : LPF output value
***********************************************************************************************************************/
float motor_filter_lpff(float f4_lpf_input, float f4_pre_lpf_output, float f4_lpf_k)
{
    float f4_temp;

    f4_temp = f4_pre_lpf_output + (f4_lpf_k * (f4_lpf_input - f4_pre_lpf_output));

    return (f4_temp);
} /* End of function motor_filter_lpff */

/***********************************************************************************************************************
* Function Name : motor_filter_limitf
* Description   : Limit with maximum limit and minimum limit
* Arguments     : f4_value - Target value
                  f4_max   - Maximum limit
                  f4_min   - Minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
float motor_filter_limitf(float f4_value, float f4_max, float f4_min)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value > f4_max)
    {
        f4_temp = f4_max;
    }
    else if (f4_value < f4_min)
    {
        f4_temp = f4_min;
    }
    else
    {
        /* Do Nothing */
    }

    return (f4_temp);
} /* End of function motor_filter_limitf */

/***********************************************************************************************************************
* Function Name : motor_filter_limit
* Description   : Limit with maximum limit and minimum limit
* Arguments     : s2_value - Target value
                  s2_max   - Maximum limit
                  s2_min   - Minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
int16_t motor_filter_limit(int16_t s2_value, int16_t s2_max, int16_t s2_min)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value > s2_max)
    {
        s2_temp = s2_max;
    }
    else if (s2_value < s2_min)
    {
        s2_temp = s2_min;
    }
    else
    {
        /* Do Nothing */
    }

    return (s2_temp);
} /* End of function motor_filter_limit */

/***********************************************************************************************************************
* Function Name : motor_filter_upper_limitf
* Description   : Limit with maximum limit
* Arguments     : f4_value - Target value
                  f4_max   - Maximum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
float motor_filter_upper_limitf(float f4_value, float f4_max)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value > f4_max)
    {
        f4_temp = f4_max;
    }

    return (f4_temp);
} /* End of function motor_filter_upper_limitf */

/***********************************************************************************************************************
* Function Name : motor_filter_upper_limit
* Description   : Limit with maximum limit
* Arguments     : s2_value - Target value
                  s2_max   - Maximum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
int16_t motor_filter_upper_limit(int16_t s2_value, int16_t s2_max)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value > s2_max)
    {
        s2_temp = s2_max;
    }

    return (s2_temp);
} /* End of function motor_filter_upper_limit */

/***********************************************************************************************************************
* Function Name : motor_filter_lower_limitf
* Description   : Limit with minimum limit
* Arguments     : f4_value - Target value
                  f4_min   - minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
float motor_filter_lower_limitf(float f4_value, float f4_min)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value < f4_min)
    {
        f4_temp = f4_min;
    }

    return (f4_temp);
} /* End of function motor_filter_lower_limitf */

/***********************************************************************************************************************
* Function Name : motor_filter_lower_limit
* Description   : Limit with minimum limit
* Arguments     : s2_value - Target value
                  s2_min   - Minimum limit
* Return Value  : Limited target value
***********************************************************************************************************************/
int16_t motor_filter_lower_limit(int16_t s2_value, int16_t s2_min)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value < s2_min)
    {
        s2_temp = s2_min;
    }

    return (s2_temp);
} /* End of function motor_filter_lower_limit */

/***********************************************************************************************************************
* Function Name : motor_filter_limitf_abs
* Description   : Limit with absolute value
* Arguments     : f4_value       - Target value
                  f4_limit_value - Limit
* Return Value  : Limited value
***********************************************************************************************************************/
float motor_filter_limitf_abs(float f4_value, float f4_limit_value)
{
    float f4_temp0;

    f4_temp0 = fabsf(f4_value);
    if (f4_temp0 > f4_limit_value)
    {
        f4_value = copysignf(f4_limit_value, f4_value);
    }

    return (f4_value);
} /* End of function motor_filter_limitf_abs */

/***********************************************************************************************************************
* Function Name : motor_filter_limit_abs
* Description   : Limit with absolute value
* Arguments     : s2_value       - Target value
                  s2_limit_value - Limit
* Return Value  : Limited value
***********************************************************************************************************************/
int16_t motor_filter_limit_abs(int16_t s2_value, int16_t s2_limit_value)
{
    int16_t s2_temp0;

    s2_temp0 = s2_value;
    if (s2_value > s2_limit_value)
    {
        s2_temp0 = s2_limit_value;
    }
    else if (s2_value < (-s2_limit_value))
    {
        s2_temp0 = - s2_limit_value;
    }
    else
    {
        /* Do nothing */
    }

    return (s2_temp0);
} /* End of function motor_filter_limit_abs */

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff_init
* Description   : Initialize Second Order LPF
* Arguments     : p_st_lpf - Second order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_second_order_lpff_init(st_2nd_order_lpf_t *p_st_lpf)
{
    p_st_lpf->f4_pre_output       = 0.0f;
    p_st_lpf->f4_pre2_output      = 0.0f;
    p_st_lpf->f4_pre_input        = 0.0f;
    p_st_lpf->f4_pre2_input       = 0.0f;
    p_st_lpf->f4_omega_t          = 0.0f;
    p_st_lpf->f4_omega2_t         = 0.0f;
    p_st_lpf->f4_omega2_t2        = 0.0f;
    p_st_lpf->f4_gain_ka          = 0.0f;
    p_st_lpf->f4_gain_kb          = 0.0f;
    p_st_lpf->f4_gain_kc          = 0.0f;
} /* End of function motor_filter_second_order_lpff_init */

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff_reset
* Description   : Reset Second Order LPF
* Arguments     : p_st_lpf - Second order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_second_order_lpff_reset(st_2nd_order_lpf_t *p_st_lpf)
{
    p_st_lpf->f4_pre_output       = 0.0f;
    p_st_lpf->f4_pre2_output      = 0.0f;
    p_st_lpf->f4_pre_input        = 0.0f;
    p_st_lpf->f4_pre2_input       = 0.0f;
} /* End of function motor_filter_second_order_lpff_reset */

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff_gain_calc
* Description   : Gain Calculation for Second Order LPF
* Arguments     : p_st_lpf        - Second order LPF structure (pointer)
*                 f4_omega        - Natural frequency
*                 f4_zeta         - Damping ratio
*                 f4_ctrl_period  - Control period
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_second_order_lpff_gain_calc(st_2nd_order_lpf_t *p_st_lpf,
                                              float f4_omega,
                                              float f4_zeta,
                                              float f4_ctrl_period)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;

    p_st_lpf->f4_omega_t   = (MTR_TWOPI * f4_omega) * f4_ctrl_period;            /* w*T */
    p_st_lpf->f4_omega2_t  = p_st_lpf->f4_omega_t * (MTR_TWOPI * f4_omega);      /* w^2*T */
    p_st_lpf->f4_omega2_t2 = p_st_lpf->f4_omega2_t * f4_ctrl_period;             /* w^2*T^2*/

    f4_temp0           = 4.0f / p_st_lpf->f4_omega2_t2;
    f4_temp1           = 4.0f * f4_zeta;
    f4_temp2           = (-f4_temp1) / p_st_lpf->f4_omega_t;
    f4_temp1           = (f4_temp1 / p_st_lpf->f4_omega_t) + 1.0f;
    p_st_lpf->f4_gain_ka = 1.0f / (f4_temp0 + f4_temp1);                        /* 1 / (4/(w^2T^2) + (4*z)/(w*T) + 1) */
    p_st_lpf->f4_gain_kb = 2.0f - (8.0f / p_st_lpf->f4_omega2_t2);              /* 2 - 8/(w^2*T^2) */
    p_st_lpf->f4_gain_kc = (4.0f / p_st_lpf->f4_omega2_t2) + (f4_temp2 + 1.0f); /* 4/(w^2*T^2) - (4*z)/(w*T) + 1 */
} /* End of function motor_filter_second_order_lpff_gain_calc */

/***********************************************************************************************************************
* Function Name : motor_filter_second_order_lpff
* Description   : Second Order LPF
* Arguments     : p_st_lpf - Second order LPF structure (pointer)
                : f4_input - Input value
* Return Value  : Filtered value
***********************************************************************************************************************/
float motor_filter_second_order_lpff(st_2nd_order_lpf_t *p_st_lpf, float f4_input)
{
    float f4_temp1;
    float f4_temp2;
    float f4_temp3;
    float f4_output;

    f4_temp1  = p_st_lpf->f4_gain_kb * p_st_lpf->f4_pre_output;
    f4_temp2  = p_st_lpf->f4_gain_kc * p_st_lpf->f4_pre2_output;
    f4_temp3  = f4_input + (2.0f * (p_st_lpf->f4_pre_input)) + p_st_lpf->f4_pre2_input;
    f4_output = p_st_lpf->f4_gain_ka * ((f4_temp3 - f4_temp1) - f4_temp2);

    p_st_lpf->f4_pre2_input = p_st_lpf->f4_pre_input;
    p_st_lpf->f4_pre_input  = f4_input;

    p_st_lpf->f4_pre2_output = p_st_lpf->f4_pre_output;
    p_st_lpf->f4_pre_output  = f4_output;

    return (f4_output);
} /* End of function motor_filter_second_order_lpff */

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff_init
* Description   : Initialize First Order LPF
* Arguments     : p_st_lpf - First order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_first_order_lpff_init(st_1st_order_lpf_t *p_st_lpf)
{
    p_st_lpf->f4_pre_output   = 0.0f;
    p_st_lpf->f4_pre_input    = 0.0f;
    p_st_lpf->f4_omega_t      = 0.0f;
    p_st_lpf->f4_gain_ka      = 0.0f;
    p_st_lpf->f4_gain_kb      = 0.0f;
} /* End of function motor_filter_first_order_lpff_init */

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff_reset
* Description   : Reset First Order LPF
* Arguments     : p_st_lpf - First order LPF structure (pointer)
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_first_order_lpff_reset(st_1st_order_lpf_t *p_st_lpf)
{
    p_st_lpf->f4_pre_output   = 0.0f;
    p_st_lpf->f4_pre_input    = 0.0f;
} /* End of function motor_filter_first_order_lpff_reset */

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff_gain_calc
* Description   : Gain Calculation for First Order LPF
* Arguments     : p_st_lpf        - First order LPF structure (pointer)
*                 f4_omega        - Natural frequency
*                 f4_ctrl_period  - Control period
* Return Value  : None
***********************************************************************************************************************/
void motor_filter_first_order_lpff_gain_calc(st_1st_order_lpf_t *p_st_lpf, float f4_omega, float f4_ctrl_period)
{
    p_st_lpf->f4_omega_t = (MTR_TWOPI * f4_omega) * f4_ctrl_period;
    p_st_lpf->f4_gain_ka = (2.0f - p_st_lpf->f4_omega_t) / (p_st_lpf->f4_omega_t + 2.0f);
    p_st_lpf->f4_gain_kb = p_st_lpf->f4_omega_t / (p_st_lpf->f4_omega_t + 2.0f);
} /* End of function motor_filter_first_order_lpff_gain_calc */

/***********************************************************************************************************************
* Function Name : motor_filter_first_order_lpff
* Description   : First Order LPF
* Arguments     : p_st_lpf - First order LPF structure (pointer)
                : f4_input - Input value
* Return Value  : Filtered value
***********************************************************************************************************************/
float motor_filter_first_order_lpff(st_1st_order_lpf_t *p_st_lpf, float f4_input)
{
    float f4_temp0;

    f4_temp0 = (p_st_lpf->f4_gain_ka * p_st_lpf->f4_pre_output);
    f4_temp0 += (p_st_lpf->f4_gain_kb * (f4_input + p_st_lpf->f4_pre_input));

    p_st_lpf->f4_pre_input = f4_input;
    p_st_lpf->f4_pre_output = f4_temp0;

    return (f4_temp0);
} /* End of function motor_filter_first_order_lpff */
