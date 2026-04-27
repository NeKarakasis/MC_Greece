/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdint.h>
#include <math.h>
#include "rm_motor_filter.h"
#include "rm_motor_fundlib.h"
#include "rm_motor_common.h"

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief   Processes a first-order low-pass filter (LPF).
 *
 * Applies a first-order low-pass filter to the input value using the previous output and filter parameter.
 *
 * @param   f4_lpf_input      Input value to be filtered.
 * @param   f4_pre_lpf_output Previous output value of the filter.
 * @param   f4_lpf_k          Filter coefficient (0 < k < 1).
 * @return  Filtered output value.
 ***********************************************************************************************************************/
float rm_motor_filter_lpff (float f4_lpf_input, float f4_pre_lpf_output, float f4_lpf_k)
{
    float f4_temp;

    f4_temp = f4_pre_lpf_output + (f4_lpf_k * (f4_lpf_input - f4_pre_lpf_output));

    return f4_temp;
}                                      /* End of function rm_motor_filter_lpff */

/***********************************************************************************************************************
 * @brief   Limit with maximum limit and minimum limit
 *
 * @param[in] f4_value Target value
 * @param[in] f4_max Maximum limit
 * @param[in] f4_min Minimum limit
 *
 * @return Limited target value
 ***********************************************************************************************************************/
float rm_motor_filter_limitf (float f4_value, float f4_max, float f4_min)
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

    return f4_temp;
}                                      /* End of function rm_motor_filter_limitf */

/***********************************************************************************************************************
 * @brief      Limits an int16_t value to specified maximum and minimum values.
 *
 * This function clamps the input integer value so that it does not exceed the provided maximum (s2_max)
 * and does not fall below the provided minimum (s2_min). If the value is within the range, it is returned unchanged.
 *
 * @param[in]  s2_value  Target value to be limited.
 * @param[in]  s2_max    Maximum allowable value.
 * @param[in]  s2_min    Minimum allowable value.
 *
 * @return     The limited value, constrained to be within [s2_min, s2_max].
 **********************************************************************************************************************/
int16_t rm_motor_filter_limit (int16_t s2_value, int16_t s2_max, int16_t s2_min)
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

    return s2_temp;
}                                      /* End of function rm_motor_filter_limit */

/***********************************************************************************************************************
 * @brief      Clamps the input floating-point value to a specified maximum limit.
 *
 * This function takes a floating-point value and a maximum allowable value.
 * If the input value exceeds the maximum, it is clamped to the maximum value.
 * Otherwise, the original value is returned.
 *
 * @param[in]  f4_value   Target value to be limited.
 * @param[in]  f4_max     Maximum allowable value.
 *
 * @return     The limited value, constrained to not exceed f4_max.
 ***********************************************************************************************************************/
float rm_motor_filter_upper_limitf (float f4_value, float f4_max)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value > f4_max)
    {
        f4_temp = f4_max;
    }

    return f4_temp;
}                                      /* End of function rm_motor_filter_upper_limitf */

/***********************************************************************************************************************
 * @brief      Clamps the input integer value to a specified maximum limit.
 *
 * This function takes an int16_t value and a maximum allowable int16_t value.
 * If the input value exceeds the maximum, it is clamped to the maximum value.
 * Otherwise, the original value is returned.
 *
 * @param[in]  s2_value   Target value to be limited.
 * @param[in]  s2_max     Maximum allowable value.
 *
 * @return     The limited value, constrained to not exceed s2_max.
 ***********************************************************************************************************************/
int16_t rm_motor_filter_upper_limit (int16_t s2_value, int16_t s2_max)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value > s2_max)
    {
        s2_temp = s2_max;
    }

    return s2_temp;
}                                      /* End of function rm_motor_filter_upper_limit */

/***********************************************************************************************************************
 * @brief      Clamps the input floating-point value to a specified minimum limit.
 *
 * This function takes a floating-point value and a minimum allowable value.
 * If the input value is less than the minimum, it is clamped to the minimum value.
 * Otherwise, the original value is returned.
 *
 * @param[in]  f4_value   Target value to be limited.
 * @param[in]  f4_min     Minimum allowable value.
 *
 * @return     The limited value, constrained to not be less than f4_min.
 ***********************************************************************************************************************/
float rm_motor_filter_lower_limitf (float f4_value, float f4_min)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value < f4_min)
    {
        f4_temp = f4_min;
    }

    return f4_temp;
}                                      /* End of function rm_motor_filter_lower_limitf */

/***********************************************************************************************************************
 * @brief      Clamps the input integer value to a specified minimum limit.
 *
 * This function takes an int16_t value and a minimum allowable int16_t value.
 * If the input value is less than the minimum, it is clamped to the minimum value.
 * Otherwise, the original value is returned.
 *
 * @param[in]  s2_value   Target value to be limited.
 * @param[in]  s2_min     Minimum allowable value.
 *
 * @return     The limited value, constrained to not be less than s2_min.
 ***********************************************************************************************************************/
int16_t rm_motor_filter_lower_limit (int16_t s2_value, int16_t s2_min)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value < s2_min)
    {
        s2_temp = s2_min;
    }

    return s2_temp;
}                                      /* End of function rm_motor_filter_lower_limit */

/***********************************************************************************************************************
 * @brief      Limits a floating-point value to a specified absolute value.
 *
 * This function clamps the input floating-point value so that its magnitude does not exceed
 * the specified limit, preserving the sign of the original value.
 *
 * @param[in]  f4_value       Target value to be limited.
 * @param[in]  f4_limit_value Absolute limit value (must be positive).
 *
 * @return     The limited value, constrained to be within [-f4_limit_value, f4_limit_value].
 ***********************************************************************************************************************/
float rm_motor_filter_limitf_abs (float f4_value, float f4_limit_value)
{
    float f4_temp0;

    f4_temp0 = fabsf(f4_value);
    if (f4_temp0 > f4_limit_value)
    {
        f4_value = copysignf(f4_limit_value, f4_value);
    }

    return f4_value;
}                                      /* End of function rm_motor_filter_limitf_abs */

/***********************************************************************************************************************
 * @brief      Limits an integer value to a specified absolute value.
 *
 * This function clamps the input int16_t value so that its magnitude does not exceed
 * the specified limit, preserving the sign of the original value.
 *
 * @param[in]  s2_value       Target value to be limited.
 * @param[in]  s2_limit_value Absolute limit value (must be positive).
 *
 * @return     The limited value, constrained to be within [-s2_limit_value, s2_limit_value].
 ***********************************************************************************************************************/
int16_t rm_motor_filter_limit_abs (int16_t s2_value, int16_t s2_limit_value)
{
    int16_t s2_temp0;

    s2_temp0 = s2_value;
    if (s2_value > s2_limit_value)
    {
        s2_temp0 = s2_limit_value;
    }
    else if (s2_value < (int16_t) (-s2_limit_value))
    {
        s2_temp0 = (int16_t) (s2_limit_value * -1);
    }
    else
    {
        /* Do nothing */
    }

    return s2_temp0;
}                                      /* End of function rm_motor_filter_limit_abs */

/***********************************************************************************************************************
 * @brief      Initializes the parameters of a second order low-pass filter (LPF).
 *
 * This function sets all internal state and gain variables of the second order LPF
 * structure to zero. It should be called before using the filter, to ensure that
 * all previous state data is cleared and the filter starts with a known state.
 *
 * @param[in,out] p_st_lpf Pointer to the second order LPF structure to initialize.
 *
 * @return     None.
 ***********************************************************************************************************************/
void rm_motor_filter_second_order_lpff_init (motor_fundlib_2nd_lpf_t * p_st_lpf)
{
    p_st_lpf->f4_pre_output  = 0.0F;
    p_st_lpf->f4_pre2_output = 0.0F;
    p_st_lpf->f4_pre_input   = 0.0F;
    p_st_lpf->f4_pre2_input  = 0.0F;
    p_st_lpf->f4_omega_t     = 0.0F;
    p_st_lpf->f4_omega2_t    = 0.0F;
    p_st_lpf->f4_omega2_t2   = 0.0F;
    p_st_lpf->f4_gain_ka     = 0.0F;
    p_st_lpf->f4_gain_kb     = 0.0F;
    p_st_lpf->f4_gain_kc     = 0.0F;
}                                      /* End of function rm_motor_filter_second_order_lpff_init */

/***********************************************************************************************************************
 * @brief      Resets the internal state of a second order low-pass filter (LPF).
 *
 * This function sets all internal state variables of the second order LPF structure to zero,
 * clearing previous input and output values. It should be called to reinitialize the filter
 * state without affecting the filter gain parameters.
 *
 * @param[in,out] p_st_lpf Pointer to the second order LPF structure to reset.
 *
 * @return     None.
 ***********************************************************************************************************************/
void rm_motor_filter_second_order_lpff_reset (motor_fundlib_2nd_lpf_t * p_st_lpf)
{
    p_st_lpf->f4_pre_output  = 0.0F;
    p_st_lpf->f4_pre2_output = 0.0F;
    p_st_lpf->f4_pre_input   = 0.0F;
    p_st_lpf->f4_pre2_input  = 0.0F;
}                                      /* End of function rm_motor_filter_second_order_lpff_reset */

/***********************************************************************************************************************
 * @brief      Calculates and sets gain parameters for a second order low-pass filter (LPF).
 *
 * This function computes the internal coefficients for a second order LPF based on the provided
 * natural frequency, damping ratio, and control period. The calculated gains are stored in the
 * motor_fundlib_2nd_lpf_t structure pointed to by p_st_lpf. This initialization is necessary
 * before using the filter for signal processing.
 *
 * @param[in,out] p_st_lpf      Pointer to the second order LPF structure to configure.
 * @param[in]     f4_omega      Natural frequency (radians/sec).
 * @param[in]     f4_zeta       Damping ratio (dimensionless).
 * @param[in]     f4_ctrl_period Control period (seconds).
 *
 * @return        None.
 ***********************************************************************************************************************/
void rm_motor_filter_second_order_lpff_gain_calc (motor_fundlib_2nd_lpf_t * p_st_lpf,
                                                  float                     f4_omega,
                                                  float                     f4_zeta,
                                                  float                     f4_ctrl_period)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;

    p_st_lpf->f4_omega_t   = (MOTOR_FUNDLIB_TWOPI * f4_omega) * f4_ctrl_period;       /* w*T */
    p_st_lpf->f4_omega2_t  = p_st_lpf->f4_omega_t * (MOTOR_FUNDLIB_TWOPI * f4_omega); /* w^2*T */
    p_st_lpf->f4_omega2_t2 = p_st_lpf->f4_omega2_t * f4_ctrl_period;                  /* w^2*T^2*/

    f4_temp0             = 4.0F / p_st_lpf->f4_omega2_t2;
    f4_temp1             = 4.0F * f4_zeta;
    f4_temp2             = (-f4_temp1) / p_st_lpf->f4_omega_t;
    f4_temp1             = (f4_temp1 / p_st_lpf->f4_omega_t) + 1.0F;
    p_st_lpf->f4_gain_ka = 1.0F / (f4_temp0 + f4_temp1);                        /* 1 / (4/(w^2T^2) + (4*z)/(w*T) + 1) */
    p_st_lpf->f4_gain_kb = 2.0F - (8.0F / p_st_lpf->f4_omega2_t2);              /* 2 - 8/(w^2*T^2) */
    p_st_lpf->f4_gain_kc = (4.0F / p_st_lpf->f4_omega2_t2) + (f4_temp2 + 1.0F); /* 4/(w^2*T^2) - (4*z)/(w*T) + 1 */
} /* End of function rm_motor_filter_second_order_lpff_gain_calc */

/***********************************************************************************************************************
 * @brief      Applies a second order low-pass filter (LPF) to the input value.
 *
 * This function processes the given input value using the configured second order LPF parameters
 * in the provided motor_fundlib_2nd_lpf_t structure. It updates the filter's internal state and returns
 * the filtered output. The filter coefficients and state variables must be properly initialized
 * before calling this function.
 *
 * @param[in,out] p_st_lpf Pointer to the second order LPF structure containing filter state and parameters.
 * @param[in]     f4_input The input value to be filtered.
 *
 * @return     The filtered output value.
 ***********************************************************************************************************************/
float rm_motor_filter_second_order_lpff (motor_fundlib_2nd_lpf_t * p_st_lpf, float f4_input)
{
    float f4_temp1;
    float f4_temp2;
    float f4_temp3;
    float f4_output;

    f4_temp1  = p_st_lpf->f4_gain_kb * p_st_lpf->f4_pre_output;
    f4_temp2  = p_st_lpf->f4_gain_kc * p_st_lpf->f4_pre2_output;
    f4_temp3  = f4_input + (2.0F * (p_st_lpf->f4_pre_input)) + p_st_lpf->f4_pre2_input;
    f4_output = p_st_lpf->f4_gain_ka * ((f4_temp3 - f4_temp1) - f4_temp2);

    p_st_lpf->f4_pre2_input = p_st_lpf->f4_pre_input;
    p_st_lpf->f4_pre_input  = f4_input;

    p_st_lpf->f4_pre2_output = p_st_lpf->f4_pre_output;
    p_st_lpf->f4_pre_output  = f4_output;

    return f4_output;
}                                      /* End of function rm_motor_filter_second_order_lpff */

/***********************************************************************************************************************
 * @brief      Initializes the parameters of a first order low-pass filter (LPF).
 *
 * This function sets all internal state and gain variables of the first order LPF
 * structure to zero. It should be called before using the filter, to ensure that
 * all previous state data is cleared and the filter starts with a known state.
 *
 * @param[in,out] p_st_lpf Pointer to the first order LPF structure to initialize.
 *
 * @return     None.
 ***********************************************************************************************************************/
void rm_motor_filter_first_order_lpff_init (motor_common_lpf_t * p_st_lpf)
{
    p_st_lpf->f4_pre_output = 0.0F;
    p_st_lpf->f4_pre_input  = 0.0F;
    p_st_lpf->f4_omega_t    = 0.0F;
    p_st_lpf->f4_gain_ka    = 0.0F;
    p_st_lpf->f4_gain_kb    = 0.0F;
}                                      /* End of function rm_motor_filter_first_order_lpff_init */

/***********************************************************************************************************************
 * @brief      Resets the internal state of a first order low-pass filter (LPF).
 *
 * This function sets all internal state variables of the first order LPF structure to zero,
 * clearing previous input and output values. It should be called to reinitialize the filter
 * state without affecting the filter gain parameters.
 *
 * @param[in,out] p_st_lpf Pointer to the first order LPF structure to reset.
 *
 * @return     None.
 ***********************************************************************************************************************/
void rm_motor_filter_first_order_lpff_reset (motor_common_lpf_t * p_st_lpf)
{
    p_st_lpf->f4_pre_output = 0.0F;
    p_st_lpf->f4_pre_input  = 0.0F;
}                                      /* End of function rm_motor_filter_first_order_lpff_reset */

/***********************************************************************************************************************
 * @brief      Calculates and sets gain parameters for a first order low-pass filter (LPF).
 *
 * This function computes the internal coefficients for a first order LPF based on the provided
 * natural frequency and control period. The calculated gains are stored in the motor_common_lpf_t
 * structure pointed to by p_st_lpf. This initialization is necessary before using the filter for
 * signal processing.
 *
 * @param[in,out] p_st_lpf      Pointer to the first order LPF structure to configure.
 * @param[in]     f4_omega      Natural frequency (radians/sec).
 * @param[in]     f4_ctrl_period Control period (seconds).
 *
 * @return        None.
 ***********************************************************************************************************************/
void rm_motor_filter_first_order_lpff_gain_calc (motor_common_lpf_t * p_st_lpf, float f4_omega, float f4_ctrl_period)
{
    p_st_lpf->f4_omega_t = (MOTOR_FUNDLIB_TWOPI * f4_omega) * f4_ctrl_period;
    p_st_lpf->f4_gain_ka = (2.0F - p_st_lpf->f4_omega_t) / (p_st_lpf->f4_omega_t + 2.0F);
    p_st_lpf->f4_gain_kb = p_st_lpf->f4_omega_t / (p_st_lpf->f4_omega_t + 2.0F);
}                                      /* End of function rm_motor_filter_first_order_lpff_gain_calc */

/***********************************************************************************************************************
 * @brief      Applies a first order low-pass filter (LPF) to the input value.
 *
 * This function processes the given input value using the configured first order LPF parameters
 * in the provided motor_common_lpf_t structure. It updates the filter's internal state and returns
 * the filtered output. The filter coefficients and state variables must be properly initialized
 * before calling this function.
 *
 * @param[in,out] p_st_lpf Pointer to the first order LPF structure containing filter state and parameters.
 * @param[in]     f4_input The input value to be filtered.
 *
 * @return     The filtered output value.
 ***********************************************************************************************************************/
float rm_motor_filter_first_order_lpff (motor_common_lpf_t * p_st_lpf, float f4_input)
{
    float f4_temp0;

    f4_temp0  = (p_st_lpf->f4_gain_ka * p_st_lpf->f4_pre_output);
    f4_temp0 += (p_st_lpf->f4_gain_kb * (f4_input + p_st_lpf->f4_pre_input));

    p_st_lpf->f4_pre_input  = f4_input;
    p_st_lpf->f4_pre_output = f4_temp0;

    return f4_temp0;
}                                      /* End of function rm_motor_filter_first_order_lpff */
