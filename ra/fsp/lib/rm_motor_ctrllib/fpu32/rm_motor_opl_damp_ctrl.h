/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RM_MOTOR_LIB_OPLDAMPCTRL_H
#define RM_MOTOR_LIB_OPLDAMPCTRL_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "rm_motor_fundlib.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

typedef struct st_motor_oldamp_sub
{
    motor_common_lpf_t st_ed_lpf;             ///< Ed LPF structure
    float              f_damp_comp_gain;      ///< The gain for open-loop damping
    float              f_fb_speed_limit_rate; ///< The limit of speed feed-back rate
} motor_oldamp_sub_t;

typedef struct st_motor_oldamp
{
    float f4_ol_id_up_step;            ///< The d-axis current reference ramping up rate [A/ms]
    float f4_ol_id_down_step;          ///< The d-axis current reference ramping down rate [A/ms]
    float f4_ol_iq_down_step_ratio;    ///< The Iq reference ramping down proportion to Iqref before open-loop
    float f4_ol_id_ref;                ///< The d-axis current reference in open-loop drive
    float f4_id_down_speed_mech_rad;   ///< The speed threshold[rad/s] to ramp down the d-axis current
    float f4_id_up_speed_mech_rad;     ///< The speed threshold[rad/s] to ramp up the d-axis current
    float f4_opl2less_sw_time;         ///< Time to switch open-loop to sensor-less [s]
    float f4_switch_phase_err_rad;     ///< Phase error to decide sensor-less switch timing [rad]
} motor_oldamp_t;

/* For Openloop damping */
typedef enum  e_motor_openloop_damping
{
    MOTOR_MCSENSOR_OPENLOOP_DAMPING_DISABLE = 0, ///< Disable openloop damping
    MOTOR_MCSENSOR_OPENLOOP_DAMPING_ENABLE  = 1  ///< Enable openloop damping
} motor_openloop_damping_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
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

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/

#endif                                 // RM_MOTOR_LIB_OPLDAMPCTRL_H
