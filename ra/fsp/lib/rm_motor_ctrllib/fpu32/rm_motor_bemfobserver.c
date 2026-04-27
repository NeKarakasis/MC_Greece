/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <stdint.h>
#include "rm_motor_fundlib.h"
#include "rm_motor_bemfobserver.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Prototype definitions
 ***********************************************************************************************************************/

void rm_motor_bemf_obs_observer(motor_bemf_observer_t  * p_bemf_obs,
                                motor_parameter_ramp_t * p_motor_params,
                                float                    f4_vd_ref,
                                float                    f4_vq_ref,
                                float                    f4_id,
                                float                    f4_iq);

float rm_motor_bemf_obs_calc_d(motor_bemf_observer_t  * p_bemf_obs,
                               motor_parameter_ramp_t * p_motor_params,
                               float                    f4_speed_rad,
                               float                    f4_iq);

float rm_motor_bemf_obs_calc_q(motor_bemf_observer_t  * p_bemf_obs,
                               motor_parameter_ramp_t * p_motor_params,
                               float                    f4_speed_rad,
                               float                    f4_id);

void rm_motor_bemf_obs_init(motor_bemf_observer_t * p_bemf_obs, motor_parameter_ramp_t * p_motor_params);

void rm_motor_bemf_obs_gain_calc(motor_parameter_ramp_t * p_motor,
                                 motor_bemf_observer_t  * p_bemf_obs,
                                 float                    f4_e_obs_omega,
                                 float                    f4_e_obs_zeta,
                                 float                    f4_ctrl_period);

void rm_motor_bemf_obs_reset(motor_bemf_observer_t * p_bemf_obs);

void rm_motor_bemf_obs_pll(motor_pll_est_t * p_pll_est, float f4_phase_err, float * p_f4_speed);

void rm_motor_bemf_obs_pll_gain_calc(motor_pll_est_t * p_pll_est,
                                     float             f4_pll_est_omega,
                                     float             f4_pll_est_zeta,
                                     float             f4_ctrl_period);

void rm_motor_bemf_obs_pll_reset(motor_pll_est_t * p_pll_est);

static void rm_motor_bemf_obs_set_gain(motor_bemf_observer_t * p_bemf_obs,
                                       float                   f4_dt,
                                       float                   f4_k1_d,
                                       float                   f4_k2_d,
                                       float                   f4_k1_q,
                                       float                   f4_k2_q);

static void rm_motor_bemf_obs_angle_speed_set_gain(motor_pll_est_t * p_pll_est, float f4_kp, float f4_ki);

/***********************************************************************************************************************
 * @brief Estimate voltage disturbance on the dq-axis using the BEMF observer.
 *
 * This function estimates the voltage disturbances on both the d-axis and q-axis of the motor using the Back
 * Electromotive Force (BEMF) observer algorithm. It updates the internal state of the observer based on the provided
 * voltage references and detected currents. The estimation is performed for both d and q axes by calculating the
 * estimated current and voltage disturbance, applying appropriate limits, and updating the observer state variables.
 *
 * @param[in,out] p_bemf_obs   Pointer to the BEMF observer structure.
 * @param[in]     f4_vd_ref    d-axis voltage reference.
 * @param[in]     f4_vq_ref    q-axis voltage reference.
 * @param[in]     f4_id        d-axis detected current.
 * @param[in]     f4_iq        q-axis detected current.
 *
 * @return        None.
 ***********************************************************************************************************************/
void rm_motor_bemf_obs_observer (motor_bemf_observer_t  * p_bemf_obs,
                                 motor_parameter_ramp_t * p_motor_params,
                                 float                    f4_vd_ref,
                                 float                    f4_vq_ref,
                                 float                    f4_id,
                                 float                    f4_iq)
{
    float f4_temp0;
    float f4_l;
    float f4_r;
    motor_bemf_obs_axis_t * paxis;

    /***************************************/
    /* Estimate d-axis voltage disturbance */
    /***************************************/
    paxis = &(p_bemf_obs->st_d_axis);
    f4_l  = p_motor_params->f4_mtr_ld;
    f4_r  = p_motor_params->f4_mtr_r;

    /* Estimate d-axis current */
    f4_temp0  = (paxis->f4_i_pre - paxis->f4_i_est_pre);
    f4_temp0 *= (f4_l * paxis->f4_k_e_obs_1);
    f4_temp0 -= (paxis->f4_i_est_pre * f4_r);
    f4_temp0 += (f4_vd_ref + paxis->f4_d_est_pre);
    f4_temp0 *= (p_bemf_obs->f4_dt / f4_l);
    f4_temp0 += paxis->f4_i_est_pre;

    /* limitation */
    f4_temp0 = rm_motor_fundlib_limitfabs(f4_temp0, paxis->f4_d_est_limit);

    /* Store estimated current */
    paxis->f4_i_est_pre = f4_temp0;

    /* Estimate d-axis voltage disturbance */
    f4_temp0        = f4_id - f4_temp0;
    f4_temp0       *= (paxis->f4_k_e_obs_2 * p_bemf_obs->f4_dt);
    f4_temp0       += paxis->f4_d_est_pre;
    paxis->f4_d_est = f4_temp0;

    /* Store estimated voltage disturbance */
    paxis->f4_d_est_pre = f4_temp0;

    /* Store current */
    paxis->f4_i_pre = f4_id;

    /***************************************/
    /* Estimate q-axis voltage disturbance */
    /***************************************/
    paxis = &(p_bemf_obs->st_q_axis);
    f4_l  = p_motor_params->f4_mtr_lq;

    /* Estimate q-axis current */
    f4_temp0  = (paxis->f4_i_pre - paxis->f4_i_est_pre);
    f4_temp0 *= (f4_l * paxis->f4_k_e_obs_1);
    f4_temp0 -= (paxis->f4_i_est_pre * f4_r);
    f4_temp0 += (f4_vq_ref + paxis->f4_d_est_pre);
    f4_temp0 *= (p_bemf_obs->f4_dt / f4_l);
    f4_temp0 += paxis->f4_i_est_pre;

    /* limitation */
    f4_temp0 = rm_motor_fundlib_limitfabs(f4_temp0, paxis->f4_d_est_limit);

    /* Store estimated current */
    paxis->f4_i_est_pre = f4_temp0;

    /* Estimate q-axis voltage disturbance */
    f4_temp0        = f4_iq - f4_temp0;
    f4_temp0       *= (paxis->f4_k_e_obs_2 * p_bemf_obs->f4_dt);
    f4_temp0       += paxis->f4_d_est_pre;
    paxis->f4_d_est = f4_temp0;

    /* Store estimated voltage disturbance */
    paxis->f4_d_est_pre = f4_temp0;

    /* Store current */
    paxis->f4_i_pre = f4_iq;
}                                      /* End of function rm_motor_estimate_bemf_observer */

/***********************************************************************************************************************
 * @brief Calculate d-axis Back Electromotive Force (BEMF).
 *
 * This function computes the d-axis BEMF based on the current motor speed and the detected q-axis current.
 * It uses the motor parameters from the BEMF observer structure, specifically the q-axis inductance and
 * the previously estimated d-axis voltage disturbance.
 *
 * @param[in]  p_bemf_obs    Pointer to the BEMF observer structure.
 * @param[in]  f4_speed_rad  Motor speed in radians per second.
 * @param[in]  f4_iq         Detected q-axis current.
 *
 * @return     D-axis BEMF value.
 ***********************************************************************************************************************/
float rm_motor_bemf_obs_calc_d (motor_bemf_observer_t  * p_bemf_obs,
                                motor_parameter_ramp_t * p_motor_params,
                                float                    f4_speed_rad,
                                float                    f4_iq)
{
    float f4_temp;
    float f4_flux_lqiq;

    f4_flux_lqiq = p_motor_params->f4_mtr_lq * f4_iq;
    f4_temp      = (f4_speed_rad * f4_flux_lqiq) - p_bemf_obs->st_d_axis.f4_d_est;

    return f4_temp;
}                                      /* End of function rm_motor_estimate_bemf_calc_d */

/***********************************************************************************************************************
 * @brief Calculate q-axis Back Electromotive Force (BEMF).
 *
 * This function computes the q-axis BEMF based on the current motor speed and the detected d-axis current.
 * It uses the motor parameters from the BEMF observer structure, specifically the d-axis inductance and
 * the previously estimated q-axis voltage disturbance.
 *
 * @param[in]  p_bemf_obs    Pointer to the BEMF observer structure.
 * @param[in]  f4_speed_rad  Motor speed in radians per second.
 * @param[in]  f4_id         Detected d-axis current.
 *
 * @return     Q-axis BEMF value.
 ***********************************************************************************************************************/
float rm_motor_bemf_obs_calc_q (motor_bemf_observer_t  * p_bemf_obs,
                                motor_parameter_ramp_t * p_motor_params,
                                float                    f4_speed_rad,
                                float                    f4_id)
{
    float f4_temp;
    float f4_flux_ldid;

    f4_flux_ldid = p_motor_params->f4_mtr_ld * f4_id;
    f4_temp      = (f4_speed_rad * (-f4_flux_ldid)) - p_bemf_obs->st_q_axis.f4_d_est;

    return f4_temp;
}                                      /* End of function rm_motor_estimate_bemf_calc_q */

/***********************************************************************************************************************
 * @brief Reset the BEMF observer state variables (excluding gains and limiters).
 *
 * This function resets the internal state variables of the Back Electromotive Force (BEMF) observer,
 * including previous current, estimated current, and voltage disturbance values for both the d-axis
 * and q-axis. The observer gains and limiters are not affected by this reset.
 *
 * @param[in] p_bemf_obs Pointer to the BEMF observer structure.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_bemf_obs_reset (motor_bemf_observer_t * p_bemf_obs)
{
    p_bemf_obs->st_d_axis.f4_i_pre     = 0.0F;
    p_bemf_obs->st_d_axis.f4_i_est_pre = 0.0F;
    p_bemf_obs->st_d_axis.f4_d_est     = 0.0F;
    p_bemf_obs->st_d_axis.f4_d_est_pre = 0.0F;
    p_bemf_obs->st_q_axis.f4_i_pre     = 0.0F;
    p_bemf_obs->st_q_axis.f4_i_est_pre = 0.0F;
    p_bemf_obs->st_q_axis.f4_d_est     = 0.0F;
    p_bemf_obs->st_q_axis.f4_d_est_pre = 0.0F;
}                                      /* End of function rm_motor_estimate_bemf_obs_reset */

/***********************************************************************************************************************
 * @brief Calculate and set BEMF observer gains and current limiters.
 *
 * This function computes the Back Electromotive Force (BEMF) observer gains based on the supplied motor parameters,
 * observer design values (natural frequency and damping ratio), and control period. It sets the computed gains for
 * both d-axis and q-axis observers, and calculates the current limitation for the voltage disturbance for each axis.
 *
 * @param[in]  p_motor         Pointer to the motor parameter structure.
 * @param[in]  p_bemf_obs      Pointer to the BEMF observer structure to set gains for.
 * @param[in]  f4_e_obs_omega  Observer natural frequency (Hz).
 * @param[in]  f4_e_obs_zeta   Observer damping ratio.
 * @param[in]  f4_ctrl_period  Control period (seconds).
 *
 * @return     None
 ***********************************************************************************************************************/
void rm_motor_bemf_obs_gain_calc (motor_parameter_ramp_t * p_motor,
                                  motor_bemf_observer_t  * p_bemf_obs,
                                  float                    f4_e_obs_omega,
                                  float                    f4_e_obs_zeta,
                                  float                    f4_ctrl_period)
{
    float f4_omega_rad;
    float f4_omega_rad_square;
    float f4_r_div_ld;
    float f4_k_ed_obs_1;
    float f4_k_ed_obs_2;
    float f4_k_eq_obs_1;
    float f4_k_eq_obs_2;

    /***** BEMF observer gain *****/
    f4_omega_rad        = MOTOR_FUNDLIB_TWOPI * f4_e_obs_omega;
    f4_omega_rad_square = f4_omega_rad * f4_omega_rad;
    f4_r_div_ld         = p_motor->f4_mtr_r / p_motor->f4_mtr_ld;
    f4_k_ed_obs_1       = (MOTOR_FUNDLIB_MULTIPLE_2 * (f4_e_obs_zeta * f4_omega_rad)) - f4_r_div_ld;
    f4_k_ed_obs_2       = f4_omega_rad_square * p_motor->f4_mtr_ld;
    f4_r_div_ld         = p_motor->f4_mtr_r / p_motor->f4_mtr_lq;
    f4_k_eq_obs_1       = (MOTOR_FUNDLIB_MULTIPLE_2 * (f4_e_obs_zeta * f4_omega_rad)) - f4_r_div_ld;
    f4_k_eq_obs_2       = f4_omega_rad_square * p_motor->f4_mtr_lq;

    /* Set gains */
    rm_motor_bemf_obs_set_gain(p_bemf_obs, f4_ctrl_period, f4_k_ed_obs_1, f4_k_ed_obs_2, f4_k_eq_obs_1, f4_k_eq_obs_2);

    /* Limit calculation */
    p_bemf_obs->st_d_axis.f4_d_est_limit = p_motor->f4_mtr_nominal_current * MOTOR_FUNDLIB_SQRT_3 *
                                           MOTOR_FUNDLIB_MULTIPLE_3;
    p_bemf_obs->st_q_axis.f4_d_est_limit = p_motor->f4_mtr_nominal_current * MOTOR_FUNDLIB_SQRT_3 *
                                           MOTOR_FUNDLIB_MULTIPLE_3;
}                                      /* End of function rm_motor_estimate_bemf_observer_gain_calc */

/***********************************************************************************************************************
 * @brief Calculates the motor electrical speed using a Phase-Locked Loop (PLL) based on the phase error.
 *
 * This function updates the speed estimation using a PLL algorithm. It takes the input phase error between
 * the estimated and actual rotor electrical angles, and applies the proportional-integral (PI) control
 * using the PLL estimator gains to compute the new estimated speed. The integral component is accumulated
 * in the estimator structure, and the result is stored in the provided speed pointer.
 *
 * @param[in,out] p_pll_est   Pointer to the PLL estimator structure.
 * @param[in]     f4_phase_err Estimated phase error (radians).
 * @param[out]    p_f4_speed   Pointer to store the computed electrical speed (rad/s).
 *
 * @return        None
 ***********************************************************************************************************************/
void rm_motor_bemf_obs_pll (motor_pll_est_t * p_pll_est, float f4_phase_err, float * p_f4_speed)
{
    float f4_temp0;
    float f4_temp1;

    f4_temp0 = -f4_phase_err;

    /* Speed calculation*/
    f4_temp1 = f4_temp0 * p_pll_est->f4_kp_est_speed;
    p_pll_est->f4_i_est_speed += (f4_temp0 * p_pll_est->f4_ki_est_speed);
    *p_f4_speed                = f4_temp1 + p_pll_est->f4_i_est_speed;
}                                      /* End of function rm_motor_estimate_speed_pll */

/***********************************************************************************************************************
 * @brief Calculates and sets PLL estimator gains for motor electrical speed estimation.
 *
 * This function computes the proportional and integral gains for the Phase-Locked Loop (PLL) speed estimator
 * based on the provided design parameters (natural frequency and damping ratio) and control period.
 * The calculated gains are then set in the PLL estimator structure.
 *
 * @param[in]  p_design_params Pointer to the structure containing PLL design parameters.
 * @param[in]  p_pll_est       Pointer to the PLL estimator structure.
 * @param[in]  f4_ctrl_period  Control period (seconds).
 *
 * @return     None
 ***********************************************************************************************************************/
void rm_motor_bemf_obs_pll_gain_calc (motor_pll_est_t * p_pll_est,
                                      float             f4_pll_est_omega,
                                      float             f4_pll_est_zeta,
                                      float             f4_ctrl_period)
{
    float f4_temp0;
    float f4_kp_speed_estimation;
    float f4_ki_speed_estimation;

    /***** PLL Speed estimate loop gain *****/
    f4_temp0               = MOTOR_FUNDLIB_TWOPI * f4_pll_est_omega;
    f4_kp_speed_estimation = MOTOR_FUNDLIB_MULTIPLE_2 * (f4_temp0 * f4_pll_est_zeta);
    f4_ki_speed_estimation = (f4_temp0 * f4_temp0) * f4_ctrl_period;

    /* Set gains */
    rm_motor_bemf_obs_angle_speed_set_gain(p_pll_est, f4_kp_speed_estimation, f4_ki_speed_estimation);
}                                      /* End of function rm_motor_estimate_speed_gain_calc */

/***********************************************************************************************************************
 * @brief Resets the integral component of the PLL speed estimator.
 *
 * This function sets the integral term of the PLL estimator to zero, effectively resetting the accumulated
 * error in speed estimation. It does not modify any gains or limiter settings in the PLL estimator structure.
 *
 * @param[in,out] p_pll_est Pointer to the PLL estimator structure to reset.
 *
 * @return None
 ***********************************************************************************************************************/
void rm_motor_bemf_obs_pll_reset (motor_pll_est_t * p_pll_est)
{
    p_pll_est->f4_i_est_speed = 0.0F;
}                                      /* End of function rm_motor_estimate_speed_reset */

/***********************************************************************************************************************
 * @brief Sets the Back Electromotive Force (BEMF) observer gains for d-axis and q-axis.
 *
 * This function assigns the control period and observer gains for both the d-axis and q-axis in the BEMF observer
 * structure. These gains are used for BEMF estimation in field-oriented control of motors.
 *
 * @param[in,out] p_bemf_obs Pointer to the BEMF observer structure to configure.
 * @param[in]     f4_dt      Control period (seconds).
 * @param[in]     f4_k1_d    Observer gain 1 for d-axis.
 * @param[in]     f4_k2_d    Observer gain 2 for d-axis.
 * @param[in]     f4_k1_q    Observer gain 1 for q-axis.
 * @param[in]     f4_k2_q    Observer gain 2 for q-axis.
 *
 * @return        None
 ***********************************************************************************************************************/
static void rm_motor_bemf_obs_set_gain (motor_bemf_observer_t * p_bemf_obs,
                                        float                   f4_dt,
                                        float                   f4_k1_d,
                                        float                   f4_k2_d,
                                        float                   f4_k1_q,
                                        float                   f4_k2_q)
{
    /* Setup control period */
    p_bemf_obs->f4_dt = f4_dt;

    /* Setup gains */
    p_bemf_obs->st_d_axis.f4_k_e_obs_1 = f4_k1_d;
    p_bemf_obs->st_d_axis.f4_k_e_obs_2 = f4_k2_d;
    p_bemf_obs->st_q_axis.f4_k_e_obs_1 = f4_k1_q;
    p_bemf_obs->st_q_axis.f4_k_e_obs_2 = f4_k2_q;
}                                      /* End of function rm_motor_bemf_obs_set_gain */

/***********************************************************************************************************************
 * @brief Sets the proportional and integral gains for the PLL angle/speed estimator.
 *
 * This function assigns the provided proportional (Kp) and integral (Ki) gains to the PLL estimator structure,
 * which are used in the speed and angle estimation control loop.
 *
 * @param[in,out] p_pll_est Pointer to the PLL estimator structure to configure.
 * @param[in]     f4_kp     Proportional gain for the PLL estimator.
 * @param[in]     f4_ki     Integral gain for the PLL estimator.
 *
 * @return        None
 ***********************************************************************************************************************/
static void rm_motor_bemf_obs_angle_speed_set_gain (motor_pll_est_t * p_pll_est, float f4_kp, float f4_ki)
{
    /* Setup gains */
    p_pll_est->f4_kp_est_speed = f4_kp;
    p_pll_est->f4_ki_est_speed = f4_ki;
}                                      /* End of function rm_motor_bemf_obs_angle_speed_set_gain */
