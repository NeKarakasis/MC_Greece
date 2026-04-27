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
#include "rm_motor_pi_control.h"

/***********************************************************************************************************************
 * @brief        Calculates the output of the PI controller using the provided control structure.
 *
 * This function computes the PI (Proportional-Integral) control output based on the given error, proportional gain,
 * integral gain, and the integrator value stored in the control structure. The output and the integrator are limited
 * by the configured integral limit value.
 *
 * @param[in]    p_st_pi_ctrl - Pointer to the PI control structure.
 * @return       PI control output value.
 ***********************************************************************************************************************/
float rm_motor_pi_ctrl (pi_ctrl_t * p_st_pi_ctrl)
{
    float f4_err;
    float f4_kp;
    float f4_ki;
    float f4_refp;
    float f4_refi;
    float f4_ref;
    float f4_ilimit;

    f4_err    = p_st_pi_ctrl->f4_err;
    f4_kp     = p_st_pi_ctrl->f4_kp;
    f4_ki     = p_st_pi_ctrl->f4_ki;
    f4_refi   = p_st_pi_ctrl->f4_refi;
    f4_ilimit = p_st_pi_ctrl->f4_ilimit;

    f4_refp  = f4_err * f4_kp;         /* Proportional part */
    f4_refi += (f4_err * f4_ki);       /* Integral part */

    /*** Integral part limit ***/
    f4_refi               = rm_motor_filter_limitf_abs(f4_refi, f4_ilimit);
    p_st_pi_ctrl->f4_refi = f4_refi;

    f4_ref = f4_refp + f4_refi;        /* PI output */
    f4_ref = rm_motor_filter_limitf_abs(f4_ref, f4_ilimit);

    return f4_ref;
} /* End of function rm_motor_pi_ctrl */

/***********************************************************************************************************************
 * @brief        Resets the PI controller integrator value.
 *
 * @param[in]    p_st_pi_ctrl - Pointer to the PI control structure.
 * @return       None
 ***********************************************************************************************************************/
void rm_motor_pi_ctrl_reset (pi_ctrl_t * p_st_pi_ctrl)
{
    p_st_pi_ctrl->f4_refi = 0.0F;
}                                      /* End of function rm_motor_pi_ctrl_reset */

/***********************************************************************************************************************
 * @brief        Sets the proportional gain (Kp) of the PI controller.
 *
 * @param[in]    p_st_pi_ctrl - Pointer to the PI control structure.
 * @param[in]    f4_kp        - The proportional gain value to set.
 * @return       None
 ***********************************************************************************************************************/
void rm_motor_pi_ctrl_kp_set (pi_ctrl_t * p_st_pi_ctrl, float f4_kp)
{
    p_st_pi_ctrl->f4_kp = f4_kp;
}                                      /* End of function rm_motor_pi_ctrl_kp_set */

/***********************************************************************************************************************
 * @brief        Sets the integral gain (Ki) of the PI controller.
 *
 * @param[in]    p_st_pi_ctrl - Pointer to the PI control structure.
 * @param[in]    f4_ki        - The integral gain value to set.
 * @return       None
 ***********************************************************************************************************************/
void rm_motor_pi_ctrl_ki_set (pi_ctrl_t * p_st_pi_ctrl, float f4_ki)
{
    p_st_pi_ctrl->f4_ki = f4_ki;
}                                      /* End of function rm_motor_pi_ctrl_ki_set */

/***********************************************************************************************************************
 * @brief        Sets the integrator value of PI control directly.
 *
 * @param[in]    p_st_pi_ctrl  - Pointer to the PI control structure.
 * @param[in]    f4_integrator - The new value to assign to the integrator.
 * @return       None
 ***********************************************************************************************************************/
void rm_motor_pi_ctrl_integrator_set (pi_ctrl_t * p_st_pi_ctrl, float f4_integrator)
{
    p_st_pi_ctrl->f4_refi = f4_integrator;
}                                      /* End of function rm_motor_pi_ctrl_integrator_set */

/***********************************************************************************************************************
 * @brief        Sets the integral limit value for PI control.
 *
 * @param[in]    p_st_pi_ctrl - Pointer to the PI control structure.
 * @param[in]    f4_ilimit    - The integral limit value to set.
 * @return       None
 ***********************************************************************************************************************/
void rm_motor_pi_ctrl_integral_limit_set (pi_ctrl_t * p_st_pi_ctrl, float f4_ilimit)
{
    p_st_pi_ctrl->f4_ilimit = f4_ilimit;
}                                      /* End of function rm_motor_pi_ctrl_integral_limit_set */

/***********************************************************************************************************************
 * @brief        Calculates the PI control output with asymmetric output limits.
 *
 * This function computes the PI (Proportional-Integral) controller output using the provided control structure,
 * applying separate upper and lower limits to the final output value. The integrator value is limited symmetrically
 * by the configured integral limit from the control structure, while the output is limited asymmetrically by the
 * specified upper and lower bounds.
 *
 * @param[in]    p_st_pi_ctrl - Pointer to the PI control structure.
 * @param[in]    f4_upperlim  - The upper limit for the PI controller output.
 * @param[in]    f4_lowerlim  - The lower limit for the PI controller output.
 *
 * @return       PI control output value constrained between the specified upper and lower limits.
 ***********************************************************************************************************************/
float rm_motor_pi_ctrl_asymmetriclim (pi_ctrl_t * p_st_pi_ctrl, float f4_upperlim, float f4_lowerlim)
{
    float f4_err;
    float f4_kp;
    float f4_ki;
    float f4_refp;
    float f4_refi;
    float f4_ref;
    float f4_ilimit;

    f4_err    = p_st_pi_ctrl->f4_err;
    f4_kp     = p_st_pi_ctrl->f4_kp;
    f4_ki     = p_st_pi_ctrl->f4_ki;
    f4_refi   = p_st_pi_ctrl->f4_refi;
    f4_ilimit = p_st_pi_ctrl->f4_ilimit;

    f4_refp  = f4_err * f4_kp;         /* Proportional part */
    f4_refi += (f4_err * f4_ki);       /* Integral part */

    /*** Integral part ***/
    f4_refi               = rm_motor_filter_limitf_abs(f4_refi, f4_ilimit);
    p_st_pi_ctrl->f4_refi = f4_refi;

    f4_ref = f4_refp + f4_refi;        /* PI output */

    /** assymmetric limit */
    f4_ref = rm_motor_filter_upper_limitf(f4_ref, f4_upperlim);
    f4_ref = rm_motor_filter_lower_limitf(f4_ref, f4_lowerlim);

    return f4_ref;
}                                      /* End of function rm_motor_pi_ctrl_asymmetriclim */

/***********************************************************************************************************************
 * @brief        Performs anti-windup control for the PI controller.
 *
 * This function implements anti-windup logic by adjusting the integral gain of the PI controller when the output
 * is saturated at the specified upper or lower bounds. It also clamps the proportional term with feedforward
 * compensation to the output limits. If the PI output exceeds the maximum limit and the integral gain is positive,
 * the integral gain is recalculated to prevent integrator windup.
 *
 * @param[in]    p_st_pi_ctrl    - Pointer to the PI control structure.
 * @param[in]    f4_ff_comp      - Feedforward compensation value for the proportional term.
 * @param[in]    f4_pi_out       - Current PI controller output value.
 * @param[in]    f4_max          - Upper output limit for the PI controller.
 * @param[in]    f4_min          - Lower output limit for the PI controller.
 *
 * @return       None
 ***********************************************************************************************************************/
void rm_motor_pi_ctrl_anti_windup (pi_ctrl_t * p_st_pi_ctrl,
                                   float       f4_ff_comp,
                                   float       f4_pi_out,
                                   float       f4_max,
                                   float       f4_min)
{
    float f4_p_val;
    float f4_i_val;

    /* Calculate proportional term including feedforward compensation */
    f4_p_val = p_st_pi_ctrl->f4_kp * p_st_pi_ctrl->f4_err + f4_ff_comp;

    /* Get integral gain/value */
    f4_i_val = p_st_pi_ctrl->f4_ki * p_st_pi_ctrl->f4_err + p_st_pi_ctrl->f4_refi;

    /* Clamp proportional value to output limits */
    if (f4_p_val >= f4_max)
    {
        f4_p_val = f4_max;
    }
    else if (f4_p_val <= f4_min)
    {
        f4_p_val = f4_min;
    }
    else
    {
        /* Do nothing */
    }

    /* Anti-windup: Prevent integrator from winding up if output is saturated */
    if ((f4_pi_out >= f4_max) && (f4_i_val > 0.0F))
    {
        f4_i_val = f4_max - f4_p_val;
        rm_motor_pi_ctrl_integrator_set(p_st_pi_ctrl, f4_i_val);
    }
    else
    {
        /* Do nothing */
    }
}                                      /* End of function rm_motor_pi_ctrl_anti_windup */
