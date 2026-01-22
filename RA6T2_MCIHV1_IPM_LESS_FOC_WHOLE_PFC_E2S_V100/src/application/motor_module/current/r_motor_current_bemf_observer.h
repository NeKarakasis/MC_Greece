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
* File Name   : r_motor_current_bemf_observer.h
* Description : Definitions of BEMF observer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.01.2025 1.00     First Release
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef R_MOTOR_CURRENT_BEMF_OBSERVER_H
#define R_MOTOR_CURRENT_BEMF_OBSERVER_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_motor_common.h"

/***********************************************************************************************************************
* Structure definitions
***********************************************************************************************************************/
typedef struct
{
    float   f4_k_e_obs_1;                       /* BEMF observer gain 1 */
    float   f4_k_e_obs_2;                       /* BEMF observer gain 2 */
    float   f4_i_pre;                           /* Previous value of current */
    float   f4_i_est_pre;                       /* Previous value of estimated current */
    float   f4_d_est;                           /* Estimated voltage disturbance */
    float   f4_d_est_pre;                       /* Previous value of voltage disturbance */
    float   f4_d_est_limit;
} st_bemf_obs_axis_t;

typedef struct
{
    float                   f4_dt;              /* Control period */
    st_motor_parameter_t    * p_st_motor;       /* Motor parameters */
    st_bemf_obs_axis_t      st_d_axis;          /* D-axis observer */
    st_bemf_obs_axis_t      st_q_axis;          /* Q-axis observer */
} st_bemf_observer_t;

typedef struct
{
    float   f4_kp_est_speed;                    /* The proportional gain for PLL */
    float   f4_ki_est_speed;                    /* The integral gain for PLL*/
    float   f4_i_est_speed;                     /* The integrator for PLL*/
} st_pll_est_t;

/***********************************************************************************************************************
* Global function definitions
***********************************************************************************************************************/
void motor_current_bemf_observer_start(st_bemf_observer_t * p_st_bemf_observer,
                                       float f4_vd_ref,
                                       float f4_vq_ref,
                                       float f4_id,
                                       float f4_iq);
float motor_current_bemf_observer_d_calc(st_bemf_observer_t * p_st_bemf_observer, float f4_speed_rad, float f4_iq);
float motor_current_bemf_observer_q_calc(st_bemf_observer_t * p_st_bemf_observer, float f4_speed_rad, float f4_id);
float motor_current_bemf_observer_amp_calc(float f4_ed, float f4_eq);
void motor_current_bemf_observer_init(st_bemf_observer_t * p_st_bemf_observer, st_motor_parameter_t * p_st_motor);
void motor_current_bemf_observer_gain_calc(st_bemf_observer_t * p_st_bemf_observer,
                                           st_motor_parameter_t * p_st_motor,
                                           float f4_e_obs_omega_hz,
                                           float f4_e_obs_zeta,
                                           float f4_ctrl_period);
void motor_current_bemf_observer_gain_set(st_bemf_observer_t * p_st_bemf_observer,
                                          float f4_dt,
                                          float f4_k1_d,
                                          float f4_k2_d,
                                          float f4_k1_q,
                                          float f4_k2_q);
void motor_current_bemf_observer_reset(st_bemf_observer_t * p_st_bemf_observer);
void motor_current_angle_speed_pll(st_pll_est_t * p_st_pll_est, float f4_phase_err, float * f4_speed);
void motor_current_angle_speed_init(st_pll_est_t * p_st_pll_est);
void motor_current_angle_speed_gain_calc(st_pll_est_t * p_st_pll_est,
                                         float f4_pll_est_omega_hz,
                                         float f4_pll_est_zeta,
                                         float f4_ctrl_period);
void motor_current_angle_speed_gain_set(st_pll_est_t * p_st_pll_est, float f4_kp, float f4_ki);
void motor_current_angle_speed_reset(st_pll_est_t * p_st_pll_est);

#endif /* R_MOTOR_CURRENT_BEMF_OBSERVER_H */
