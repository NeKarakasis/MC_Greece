/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include "rm_motor_opl_damp_ctrl.h"
#include "rm_motor_fundlib.h"
#include "rm_motor_filter.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/
float rm_motor_opl_damp_ctrl(motor_oldamp_sub_t * p_opl_damp, float f4_ed, float f4_speed_ref);
void  rm_motor_opl_damp_init(motor_oldamp_sub_t * p_opl_damp, float f4_fb_speed_limit_rate);
void  rm_motor_opl_damp_reset(motor_oldamp_sub_t * p_opl_damp);
void  rm_motor_opl_damp_r_set_gain(motor_oldamp_sub_t * p_opl_damp,
                                   uint16_t             u2_pp,
                                   float                f4_ke,
                                   float                f4_j,
                                   float                f4_zeta,
                                   float                f4_ed_hpf_fc,
                                   float                f4_opl_current,
                                   float                f4_id_down_speed,
                                   float                f4_tc);
void rm_motor_opl_damp_set_limit(motor_oldamp_sub_t * p_opl_damp, float f4_limit_rate);

/***********************************************************************************************************************
 * @brief Open-loop damping control for motor speed.
 *
 * This function applies open-loop damping control using the provided control structure and parameters.
 *
 * @param[in]  p_opl_damp    Pointer to the damping control structure.
 * @param[in]  f4_ed         d-axis BEMF.
 * @param[in]  f4_speed_ref  Reference speed.
 *
 * @return Feedback value for reference speed.
 ***********************************************************************************************************************/
float rm_motor_opl_damp_ctrl (motor_oldamp_sub_t * p_opl_damp, float f4_ed, float f4_speed_ref)
{
    float f4_temp0;
    float f4_temp_ed_lpf_output;
    float f4_temp_delta_ed;
    float f4_temp_damp_comp_gain;
    float f4_temp_damp_comp_speed;

    /* HPF for ed */
    f4_temp_ed_lpf_output = rm_motor_filter_first_order_lpff(&p_opl_damp->st_ed_lpf, f4_ed);
    f4_temp_delta_ed      = f4_ed - f4_temp_ed_lpf_output;

    /* Calculate feedback speed */
    if (f4_speed_ref < 0.0F)
    {
        f4_temp_damp_comp_gain = -(p_opl_damp->f_damp_comp_gain);
    }
    else if (f4_speed_ref > 0.0F)
    {
        f4_temp_damp_comp_gain = p_opl_damp->f_damp_comp_gain;
    }
    else
    {
        f4_temp_damp_comp_gain = 0.0F;
    }

    f4_temp_damp_comp_speed = f4_temp_delta_ed * f4_temp_damp_comp_gain;

    /* Feedback speed limit */
    f4_temp0                = f4_speed_ref * p_opl_damp->f_fb_speed_limit_rate;
    f4_temp0                = fabsf(f4_temp0);
    f4_temp_damp_comp_speed = rm_motor_fundlib_limitfabs(f4_temp_damp_comp_speed, f4_temp0);

    return f4_temp_damp_comp_speed;
}                                      /* End of function rm_motor_opl_damp_ctrl */

/***********************************************************************************************************************
 * @brief Initializes open-loop damping control.
 *
 * Sets the initial feedback speed limiter rate and resets the internal state of the open-loop damping control structure.
 *
 * @param[in,out] p_opl_damp             Pointer to the open-loop damping control structure.
 * @param[in]     f4_fb_speed_limit_rate Rate of reference speed for feedback speed limiter.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_opl_damp_init (motor_oldamp_sub_t * p_opl_damp, float f4_fb_speed_limit_rate)
{
    /* Setup initial limiters */
    rm_motor_opl_damp_set_limit(p_opl_damp, f4_fb_speed_limit_rate);
}                                      /* End of function rm_motor_opl_damp_init */

/***********************************************************************************************************************
 * @brief Resets the internal state of the open-loop damping control (excluding gains and limiters).
 *
 * This function clears the previous input and output values of the ed low-pass filter in the open-loop
 * damping control structure. Gains and limiters are not affected.
 *
 * @param[in,out] p_opl_damp Pointer to the open-loop damping control structure.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_opl_damp_reset (motor_oldamp_sub_t * p_opl_damp)
{
    p_opl_damp->st_ed_lpf.f4_pre_output = 0.0F;
    p_opl_damp->st_ed_lpf.f4_pre_input  = 0.0F;
}                                      /* End of function rm_motor_opl_damp_reset */

/***********************************************************************************************************************
 * @brief Designs and sets open-loop damping control gains.
 *
 * This function calculates and sets the gains required for open-loop damping control in motor applications.
 * The gains are determined using the provided motor parameters and control settings.
 *
 * @param[in,out] p_opl_damp        Pointer to the open-loop damping control structure.
 * @param[in]     u2_pp             Number of pole pairs.
 * @param[in]     f4_ke             Magnetic flux [Wb].
 * @param[in]     f4_j              Inertia [kgm^2].
 * @param[in]     f4_zeta           Damping ratio of open-loop damping control.
 * @param[in]     f4_ed_hpf_fc      Cutoff frequency of HPF for ed [Hz].
 * @param[in]     f4_opl_current    The current (Id reference) [A] in open-loop.
 * @param[in]     f4_id_down_speed  Sensor-less control switch speed [rad/s].
 * @param[in]     f4_tc             Control period [s].
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_opl_damp_r_set_gain (motor_oldamp_sub_t * p_opl_damp,
                                   uint16_t             u2_pp,
                                   float                f4_ke,
                                   float                f4_j,
                                   float                f4_zeta,
                                   float                f4_ed_hpf_fc,
                                   float                f4_opl_current,
                                   float                f4_id_down_speed,
                                   float                f4_tc)
{
    float f4_temp0;

    /* Setup gains */
    f4_temp0  = MOTOR_FUNDLIB_MULTIPLE_2 * (u2_pp * f4_zeta);
    f4_temp0 *= sqrtf((f4_ke * f4_opl_current) / f4_j);
    p_opl_damp->f_damp_comp_gain = f4_temp0 / (f4_ke * f4_id_down_speed);
    f4_temp0 = (MOTOR_FUNDLIB_TWOPI * f4_ed_hpf_fc) * f4_tc;
    p_opl_damp->st_ed_lpf.f4_omega_t = f4_temp0;
    p_opl_damp->st_ed_lpf.f4_gain_ka = (MOTOR_FUNDLIB_MULTIPLE_2 - f4_temp0) / (MOTOR_FUNDLIB_MULTIPLE_2 + f4_temp0);
    p_opl_damp->st_ed_lpf.f4_gain_kb = f4_temp0 / (MOTOR_FUNDLIB_MULTIPLE_2 + f4_temp0);
}                                      /* End of function rm_motor_opl_damp_r_set_gain */

/***********************************************************************************************************************
 * @brief Sets open-loop damping control limiters.
 *
 * This function configures the feedback speed limiter rate in the open-loop damping control structure.
 *
 * @param[in,out] p_opl_damp   Pointer to the open-loop damping control structure.
 * @param[in]     f4_limit_rate Rate of reference speed for feedback speed limiter.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_opl_damp_set_limit (motor_oldamp_sub_t * p_opl_damp, float f4_limit_rate)
{
    /* Setup limiters */
    p_opl_damp->f_fb_speed_limit_rate = f4_limit_rate;
}                                      /* End of function rm_motor_opl_damp_set_limit */
