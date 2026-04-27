/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_PI_CONTROL_H
#define RM_MOTOR_LIB_PI_CONTROL_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Structure definitions
 ***********************************************************************************************************************/
typedef struct st_pi_ctrl
{
    float f4_err;                      /* Error */
    float f4_kp;                       /* Gain of proportional term */
    float f4_ki;                       /* Gain of integral term */
    float f4_refi;                     /* Buffer of integral term */
    float f4_ilimit;                   /* Limit of integral term */
} pi_ctrl_t;

/***********************************************************************************************************************
 * Global function definitions
 ***********************************************************************************************************************/
float rm_motor_pi_ctrl(pi_ctrl_t * p_st_pi_ctrl);
void  rm_motor_pi_ctrl_reset(pi_ctrl_t * p_st_pi_ctrl);
void  rm_motor_pi_ctrl_kp_set(pi_ctrl_t * p_st_pi_ctrl, float f4_kp);
void  rm_motor_pi_ctrl_ki_set(pi_ctrl_t * p_st_pi_ctrl, float f4_ki);
void  rm_motor_pi_ctrl_integrator_set(pi_ctrl_t * p_st_pi_ctrl, float f4_integrator);
void  rm_motor_pi_ctrl_integral_limit_set(pi_ctrl_t * p_st_pi_ctrl, float f4_ilimit);
float rm_motor_pi_ctrl_asymmetriclim(pi_ctrl_t * p_st_pi_ctrl, float f4_upperlim, float f4_lowerlim);
void  rm_motor_pi_ctrl_anti_windup(pi_ctrl_t * p_st_pi_ctrl,
                                   float       f4_ff_comp,
                                   float       f4_pi_out,
                                   float       f4_max,
                                   float       f4_min);

#endif                                 /* RM_MOTOR_LIB_PI_CONTROL_H */
