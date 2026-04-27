/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_BEMF_OBSERVER_H
#define RM_MOTOR_BEMF_OBSERVER_H

/*********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/

typedef struct st_motor_bemf_obs_axis
{
    float f4_k_e_obs_1;                ///< BEMF observer gain 1
    float f4_k_e_obs_2;                ///< BEMF observer gain 2
    float f4_i_pre;                    ///< Previous value of current
    float f4_i_est_pre;                ///< Previous value of estimated current
    float f4_d_est;                    ///< Estimated voltage disturbance
    float f4_d_est_pre;                ///< Previous value of voltage disturbance
    float f4_d_est_limit;              ///< Integration limit value of voltage disturbance estimate
} motor_bemf_obs_axis_t;

typedef struct st_motor_bemf_observer
{
    float                 f4_dt;       ///< Control period
    motor_bemf_obs_axis_t st_d_axis;   ///< D-axis observer
    motor_bemf_obs_axis_t st_q_axis;   ///< Q-axis observer
} motor_bemf_observer_t;

typedef struct st_motor_pll_est
{
    float f4_kp_est_speed;             ///< The proportional gain for PLL
    float f4_ki_est_speed;             ///< The integral gain for PLL
    float f4_i_est_speed;              ///< The integrator for PLL
} motor_pll_est_t;

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
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

#endif                                 /* RM_MOTOR_BEMF_OBSERVER_H */
