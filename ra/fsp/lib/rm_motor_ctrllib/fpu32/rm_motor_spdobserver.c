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
#include "rm_motor_fundlib.h"
#include "rm_motor_spdobserver.h"
#include "rm_motor_filter.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define     MOTOR_SPEED_OBSERVER_HPF_OMEGA    (1.0F) /* natural frequency for speed observer HPF */

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/
void  rm_motor_spdobserver_init(motor_spdobserver_t * p_sob);
void  rm_motor_spdobserver_reset(motor_spdobserver_t * p_sob);
float rm_motor_spdobserver(motor_spdobserver_t    * p_sob,
                           motor_parameter_ramp_t * p_motor,
                           float                    f4_iq_ref,
                           float                    f4_speed_rad);

/***********************************************************************************************************************
 * Prototype definition of static functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief Initializes the internal variables of the speed observer.
 *
 * This function sets all internal state variables of the speed observer structure to their default values,
 * preparing it for operation. It also initializes the associated first-order low-pass filter.
 *
 * @param[in,out] p_sob  Pointer to the speed observer structure.
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_spdobserver_init (motor_spdobserver_t * p_sob)
{
    /* speed observer */
    p_sob->f4_speed_rad             = 0.0F;
    p_sob->f4_ref_torque            = 0.0F;
    p_sob->f4_ref_pre_torque        = 0.0F;
    p_sob->f4_ref_speed_rad         = 0.0F;
    p_sob->f4_ref_pre_speed_rad     = 0.0F;
    p_sob->f4_hpf_ref_speed_rad     = 0.0F;
    p_sob->f4_hpf_ref_pre_speed_rad = 0.0F;
    p_sob->f4_hpf_omega             = MOTOR_SPEED_OBSERVER_HPF_OMEGA;
    rm_motor_filter_second_order_lpff_init(&p_sob->st_lpf);
}                                      /* End of function rm_motor_spdobserver_init */

/***********************************************************************************************************************
 * @brief Resets the internal variables of the speed observer.
 *
 * This function clears all state variables within the speed observer structure, preparing it for a fresh estimation cycle.
 * It also resets the associated second-order low-pass filter.
 *
 * @param[in,out] p_sob  Pointer to the speed observer structure.
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_spdobserver_reset (motor_spdobserver_t * p_sob)
{
    /* speed observer */
    p_sob->f4_speed_rad             = 0.0F;
    p_sob->f4_ref_torque            = 0.0F;
    p_sob->f4_ref_pre_torque        = 0.0F;
    p_sob->f4_ref_speed_rad         = 0.0F;
    p_sob->f4_ref_pre_speed_rad     = 0.0F;
    p_sob->f4_hpf_ref_speed_rad     = 0.0F;
    p_sob->f4_hpf_ref_pre_speed_rad = 0.0F;
    rm_motor_filter_second_order_lpff_reset(&p_sob->st_lpf);
}                                      /* End of function rm_motor_spdobserver_reset */

/***********************************************************************************************************************
 * @brief Calculates and sets the gain parameters for the motor speed observer.
 *
 * This function computes the necessary gain values for both the high-pass filter (HPF) and the second-order low-pass filter (LPF) used in the speed observer.
 * The calculations are based on the observer's natural frequency, damping ratio, and the control period of the speed loop.
 *
 * @param[in,out] p_sob               Pointer to the speed observer structure.
 * @param[in]     f4_observer_omega   Observer natural frequency [rad/s].
 * @param[in]     f4_observer_zeta    Observer damping ratio.
 * @param[in]     f4_outer_ctrl_period Control period for speed loop [s].
 *
 * @return None.
 ***********************************************************************************************************************/
void rm_motor_spdobserver_gain_calc (motor_spdobserver_t * p_sob,
                                     float                 f4_observer_omega,
                                     float                 f4_observer_zeta,
                                     float                 f4_outer_ctrl_period)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;

    /* speed observer HPF */
    f4_temp0         = MOTOR_FUNDLIB_TWOPI * p_sob->f4_hpf_omega;
    f4_temp1         = MOTOR_FUNDLIB_MULTIPLE_2 / f4_outer_ctrl_period;
    p_sob->f4_hpf_k1 = 1.0F / (f4_temp0 + f4_temp1);
    p_sob->f4_hpf_k2 = f4_temp1;
    p_sob->f4_hpf_k3 = -(1.0F - f4_temp1);

    /* speed observer */
    p_sob->st_lpf.f4_omega_t =
        (MOTOR_FUNDLIB_TWOPI * f4_observer_omega) * f4_outer_ctrl_period;
    p_sob->st_lpf.f4_omega2_t =
        p_sob->st_lpf.f4_omega_t * (MOTOR_FUNDLIB_TWOPI * f4_observer_omega);
    p_sob->st_lpf.f4_omega2_t2 = p_sob->st_lpf.f4_omega2_t * f4_outer_ctrl_period;

    f4_temp0     = f4_observer_zeta * p_sob->st_lpf.f4_omega_t;
    p_sob->f4_k1 = (MOTOR_FUNDLIB_MULTIPLE_2 * (f4_temp0 + 1.0F)) / p_sob->st_lpf.f4_omega2_t;
    p_sob->f4_k2 = (MOTOR_FUNDLIB_MULTIPLE_2 * (f4_temp0 - 1.0F)) / p_sob->st_lpf.f4_omega2_t;

    f4_temp0                 = MOTOR_FUNDLIB_MULTIPLE_4 / p_sob->st_lpf.f4_omega2_t2;
    f4_temp1                 = MOTOR_FUNDLIB_MULTIPLE_4 * f4_observer_zeta;
    f4_temp2                 = (f4_temp1 / p_sob->st_lpf.f4_omega_t) + 1.0F;
    f4_temp1                 = -f4_temp1 / p_sob->st_lpf.f4_omega_t;
    p_sob->st_lpf.f4_gain_ka = 1.0F / (f4_temp0 + f4_temp2);
    p_sob->st_lpf.f4_gain_kb = MOTOR_FUNDLIB_MULTIPLE_2 - (MOTOR_FUNDLIB_MULTIPLE_8 / p_sob->st_lpf.f4_omega2_t2);
    p_sob->st_lpf.f4_gain_kc = (MOTOR_FUNDLIB_MULTIPLE_4 / p_sob->st_lpf.f4_omega2_t2) + (f4_temp1 + 1.0F);
}                                      /* End of function rm_motor_spdobserver_gain_calc */

/***********************************************************************************************************************
 * @brief Speed observer function.
 *
 * Calculates the estimated speed of the motor using a high-pass filter (HPF) and a second-order low-pass filter (LPF).
 *
 * @param[in,out] p_sob       Pointer to the speed observer structure.
 * @param[in]     p_motor     Pointer to the motor parameter structure.
 * @param[in]     f4_iq_ref   Reference iq current [A].
 * @param[in]     f4_speed_mech Mechanical Motor speed [rad/s].
 *
 * @return Estimated mechanical speed from the observer [rad/s].
 ***********************************************************************************************************************/
float rm_motor_spdobserver (motor_spdobserver_t    * p_sob,
                            motor_parameter_ramp_t * p_motor,
                            float                    f4_iq_ref,
                            float                    f4_speed_mech)
{
    float f4_temp0;
    float f4_temp1;
    float f4_temp2;
    float f4_sob_ref_speed;

    /* HPF */
    p_sob->f4_ref_pre_torque        = p_sob->f4_ref_torque;
    p_sob->f4_ref_torque            = f4_iq_ref * p_motor->u2_mtr_pp * p_motor->f4_mtr_m;
    f4_temp1                        = p_sob->f4_hpf_k2 * (p_sob->f4_ref_torque - p_sob->f4_ref_pre_torque);
    p_sob->f4_hpf_ref_pre_speed_rad = p_sob->f4_hpf_ref_speed_rad;
    f4_temp2                        = p_sob->f4_hpf_k3 * p_sob->f4_hpf_ref_pre_speed_rad;
    p_sob->f4_hpf_ref_speed_rad     = p_sob->f4_hpf_k1 * (f4_temp1 + f4_temp2);

    f4_temp0 = p_motor->u2_mtr_pp / p_motor->f4_mtr_j;
    f4_temp1 = f4_temp0 * p_sob->f4_hpf_ref_speed_rad;
    f4_temp2 = f4_temp0 * p_sob->f4_hpf_ref_pre_speed_rad;

    /* 2nd LPF */
    f4_temp1 = p_sob->f4_k1 * f4_temp1;
    f4_temp2 = p_sob->f4_k2 * f4_temp2;
    p_sob->f4_ref_pre_speed_rad = p_sob->f4_ref_speed_rad;
    p_sob->f4_ref_speed_rad     = f4_temp1 + f4_temp2 - p_sob->f4_ref_pre_speed_rad;

    p_sob->f4_speed_rad = p_sob->f4_ref_speed_rad + f4_speed_mech;

    /* 2nd order LPF */
    f4_sob_ref_speed = rm_motor_filter_second_order_lpff(&p_sob->st_lpf, p_sob->f4_speed_rad);

    return f4_sob_ref_speed;
}                                      /* End of function rm_motor_spdobserver */
