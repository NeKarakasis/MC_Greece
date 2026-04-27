/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief Limit a float value to the specified absolute limit.
 *
 * This function constrains the input value to the range [-f4_limit_value, f4_limit_value].
 * If the absolute value of f4_value exceeds f4_limit_value, it is set to the corresponding
 * positive or negative limit.
 *
 * @param[in]  f4_value       Target value to be limited.
 * @param[in]  f4_limit_value Absolute limit.
 *
 * @return Limited value within [-f4_limit_value, f4_limit_value].
 ***********************************************************************************************************************/
float rm_motor_fundlib_limitfabs (float f4_value, float f4_limit_value)
{
    float f4_temp0;

    f4_temp0 = fabsf(f4_value);
    if (f4_temp0 > f4_limit_value)
    {
        f4_value = copysignf(f4_limit_value, f4_value);
    }

    return f4_value;
}                                      /* End of function rm_motor_fundlib_limitfabs */

/***********************************************************************************************************************
 * @brief Limit a float value to specified maximum and minimum limits.
 *
 * This function restricts the input value to be within the provided minimum and maximum limits.
 * If the value is greater than the maximum, the maximum is returned; if less than the minimum, the minimum
 * is returned; otherwise, the value itself is returned.
 *
 * @param[in]  f4_value Target value to be limited.
 * @param[in]  f4_max   Maximum limit.
 * @param[in]  f4_min   Minimum limit.
 *
 * @return Limited target value within [f4_min, f4_max].
 **********************************************************************************************************************/
float rm_motor_fundlib_limitf (float f4_value, float f4_max, float f4_min)
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
}                                      /* End of function rm_motor_fundlib_limitf */

/***********************************************************************************************************************
 * @brief Limit an int16_t value to specified maximum and minimum limits.
 *
 * This function restricts the input value to be within the provided minimum and maximum limits.
 * If the value is greater than the maximum, the maximum is returned; if less than the minimum, the minimum
 * is returned; otherwise, the value itself is returned.
 *
 * @param[in]  s2_value Target value to be limited.
 * @param[in]  s2_max   Maximum limit.
 * @param[in]  s2_min   Minimum limit.
 *
 * @return Limited target value within [s2_min, s2_max].
 **********************************************************************************************************************/
int16_t rm_motor_fundlib_limit (int16_t s2_value, int16_t s2_max, int16_t s2_min)
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
}                                      /* End of function rm_motor_fundlib_limit */

/***********************************************************************************************************************
 * @brief Limit a float value to a specified maximum limit.
 *
 * This function restricts the input value to be no greater than the provided maximum limit.
 * If the value is greater than the maximum, the maximum is returned; otherwise, the value itself is returned.
 *
 * @param[in]  f4_value Target value to be limited.
 * @param[in]  f4_max   Maximum limit.
 *
 * @return Limited target value, not greater than f4_max.
 **********************************************************************************************************************/
float rm_motor_fundlib_upper_limitf (float f4_value, float f4_max)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value > f4_max)
    {
        f4_temp = f4_max;
    }

    return f4_temp;
}                                      /* End of function rm_motor_fundlib_upper_limitf */

/***********************************************************************************************************************
 * @brief Limit an int16_t value to a specified maximum limit.
 *
 * This function restricts the input value to be no greater than the provided maximum limit.
 * If the value is greater than the maximum, the maximum is returned; otherwise, the value itself is returned.
 *
 * @param[in]  s2_value Target value to be limited.
 * @param[in]  s2_max   Maximum limit.
 *
 * @return Limited target value, not greater than s2_max.
 **********************************************************************************************************************/
int16_t rm_motor_fundlib_upper_limit (int16_t s2_value, int16_t s2_max)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value > s2_max)
    {
        s2_temp = s2_max;
    }

    return s2_temp;
}                                      /* End of function rm_motor_fundlib_upper_limit */

/***********************************************************************************************************************
 * @brief Limit a float value to a specified minimum limit.
 *
 * This function restricts the input value to be no less than the provided minimum limit.
 * If the value is less than the minimum, the minimum is returned; otherwise, the value itself is returned.
 *
 * @param[in]  f4_value Target value to be limited.
 * @param[in]  f4_min   Minimum limit.
 *
 * @return Limited target value, not less than f4_min.
 **********************************************************************************************************************/
float rm_motor_fundlib_lower_limitf (float f4_value, float f4_min)
{
    float f4_temp;

    f4_temp = f4_value;
    if (f4_value < f4_min)
    {
        f4_temp = f4_min;
    }

    return f4_temp;
}                                      /* End of function rm_motor_fundlib_lower_limitf */

/***********************************************************************************************************************
 * @brief Limit an int16_t value to a specified minimum.
 *
 * This function restricts the input value to be no less than the given minimum limit.
 * If the value is less than the minimum, the minimum is returned; otherwise, the value itself is returned.
 *
 * @param[in]  s2_value Target value to be limited.
 * @param[in]  s2_min   Minimum limit.
 *
 * @return Limited target value, not less than s2_min.
 **********************************************************************************************************************/
int16_t rm_motor_fundlib_lower_limit (int16_t s2_value, int16_t s2_min)
{
    int16_t s2_temp;

    s2_temp = s2_value;
    if (s2_value < s2_min)
    {
        s2_temp = s2_min;
    }

    return s2_temp;
}                                      /* End of function rm_motor_fundlib_lower_limit */

/***********************************************************************************************************************
 * @brief Limit a float value to the specified absolute limit.
 *
 * This function constrains the input value to the range [-f4_limit_value, f4_limit_value].
 * If the absolute value of f4_value exceeds f4_limit_value, it is set to the corresponding
 * positive or negative limit.
 *
 * @param[in]  f4_value       Target value to be limited.
 * @param[in]  f4_limit_value Absolute limit.
 *
 * @return Limited value within [-f4_limit_value, f4_limit_value].
 **********************************************************************************************************************/
float rm_motor_fundamental_limitf_abs (float f4_value, float f4_limit_value)
{
    float f4_temp0;

    f4_temp0 = fabsf(f4_value);
    if (f4_temp0 > f4_limit_value)
    {
        f4_value = copysignf(f4_limit_value, f4_value);
    }

    return f4_value;
}                                      /* End of function rm_motor_fundamental_limitf_abs */

/***********************************************************************************************************************
 * @brief Limit an int16_t value to the specified absolute limit.
 *
 * This function constrains the input value to the range [-s2_limit_value, s2_limit_value].
 * If the absolute value of s2_value exceeds s2_limit_value, it is set to the corresponding
 * positive or negative limit.
 *
 * @param[in]  s2_value       Target value to be limited.
 * @param[in]  s2_limit_value Absolute limit.
 *
 * @return Limited value within [-s2_limit_value, s2_limit_value].
 **********************************************************************************************************************/
int16_t rm_motor_fundlib_limit_abs (int16_t s2_value, int16_t s2_limit_value)
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
}                                      /* End of function rm_motor_fundlib_limit_abs */

/***********************************************************************************************************************
 * @brief Compensates for current control sampling delay in motor angle calculation.
 *
 * This function adjusts the given motor angle (electrical) by compensating for the sampling delay
 * based on the motor speed, control period, and a magnification value. The result is wrapped to
 * the range [0, 2π).
 *
 * @param[in]  f4_angle_rad   Motor angle (electrical) in radians.
 * @param[in]  f4_speed_rad   Motor speed (electrical) in radians per second.
 * @param[in]  f4_ctrl_period Motor current control period.
 * @param[in]  f4_magnitude   Period magnification value for sampling delay compensation.
 *
 * @return Compensated motor angle (electrical) in radians.
 **********************************************************************************************************************/
float rm_motor_sample_delay_comp (float f4_angle_rad, float f4_speed_rad, float f4_ctrl_period, float f4_magnitude)
{
    float f4_comp_angle_rad = 0.0F;

    f4_comp_angle_rad = f4_speed_rad * f4_ctrl_period * f4_magnitude;
    f4_comp_angle_rad = f4_angle_rad + f4_comp_angle_rad;

    /* Limit angle value of one rotation */
    if (f4_comp_angle_rad >= MOTOR_FUNDLIB_TWOPI)
    {
        f4_comp_angle_rad -= MOTOR_FUNDLIB_TWOPI;
    }
    else if (f4_comp_angle_rad < 0.0F)
    {
        f4_comp_angle_rad += MOTOR_FUNDLIB_TWOPI;
    }
    else
    {
        /* Do nothing */
    }

    return f4_comp_angle_rad;
}                                      /* End of function rm_motor_sample_delay_comp */
